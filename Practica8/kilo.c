#ifdef __linux__
#define _POSIX_C_SOURCE 200809L
#endif

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>

#define EDITOR_QUIT_TIMES 3

typedef struct erow {
    int idx, size, rsize;
    char *chars, *render;
    unsigned char *hl;
    int hl_oc;
} erow;

struct editorConfig {
    int cx ,cy, rowoff, coloff, screenrows, screencols, numrows, rawmode;
    erow *row;
    int dirty;
    char *filename, statusmsg[80];
    time_t statusmsg_time;
};

struct abuf {
    char *b;
    int len;
};

enum KEY_ACTION{
        KEY_NULL = 0,
        CTRL_C = 3,
        CTRL_H = 8,
        TAB = 9,
        CTRL_L = 12,
        ENTER = 13,
        CTRL_Q = 17,
        CTRL_S = 19,
        ESC = 27,
        BACKSPACE =  127,
        ARROW_LEFT = 1000,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,
        DEL_KEY,
        HOME_KEY,
        END_KEY,
        PAGE_UP,
        PAGE_DOWN
};

static struct editorConfig E;
static struct termios orig_termios;

void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    vsnprintf(E.statusmsg,sizeof(E.statusmsg),fmt,ap);
    va_end(ap);
    E.statusmsg_time = time(NULL);
}

void editorAtExit() {
    if (E.rawmode) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        E.rawmode = 0;
    }
}

int enableRawMode(int fd) {
    struct termios raw;
    if (E.rawmode) return 0;
    if (!isatty(STDIN_FILENO)) goto fatal;
    atexit(editorAtExit);
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;
    raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
    E.rawmode = 1;
    return 0;
fatal:
    errno = ENOTTY;
    return -1;
}

int editorReadKey(int fd) {
    int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);
    while(1) switch(c) {
        case ESC:
            if (read(fd,seq,1) == 0) return ESC;
            if (read(fd,seq+1,1) == 0) return ESC;
            if (seq[0] == '[')
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(fd,seq+2,1) == 0) return ESC;
                    if (seq[2] == '~') switch(seq[1]) {
                            case '3': return DEL_KEY;
                            case '5': return PAGE_UP;
                            case '6': return PAGE_DOWN;
                        }
                } else switch(seq[1]) {
                        case 'A': return ARROW_UP;
                        case 'B': return ARROW_DOWN;
                        case 'C': return ARROW_RIGHT;
                        case 'D': return ARROW_LEFT;
                        case 'H': return HOME_KEY;
                        case 'F': return END_KEY;
                    }
            else if (seq[0] == 'O') switch(seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
                }
            break;
        default:
            return c;
    }
}

int getCursorPosition(int ifd, int ofd, int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;
    if (write(ofd, "\x1b[6n", 4) != 4) return -1;
    while (i < sizeof(buf)-1) {
        if (read(ifd,buf+i,1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != ESC || buf[1] != '[') return -1;
    if (sscanf(buf+2,"%d;%d",rows,cols) != 2) return -1;
    return 0;
}

int getWindowSize(int ifd, int ofd, int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        int orig_row, orig_col, retval;
        retval = getCursorPosition(ifd,ofd,&orig_row,&orig_col);
        if (retval == -1) goto failed;
        if (write(ofd,"\x1b[999C\x1b[999B",12) != 12) goto failed;
        retval = getCursorPosition(ifd,ofd,rows,cols);
        if (retval == -1) goto failed;
        char seq[32];
        snprintf(seq,32,"\x1b[%d;%dH",orig_row,orig_col);
        if (write(ofd,seq,strlen(seq)) == -1) goto failed;
        return 0;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
failed:
    return -1;
}

int is_separator(int c) {
    return c == '\0' || isspace(c) || strchr(",.()+-/*=~%[];",c) != NULL;
}

int editorRowHasOpenComment(erow *row) {
    if (row->hl && row->rsize && (row->rsize < 2 || (row->render[row->rsize-2] != '*'
        || row->render[row->rsize-1] != '/'))) return 1;
    return 0;
}

void editorUpdateSyntax(erow *row) {
    row->hl = realloc(row->hl,row->rsize);
    memset(row->hl, 0, row->rsize);
    if (E.filename == NULL) return;
    int i, prev_sep, in_string, in_comment;
    char *p = row->render;
    i = in_string = in_comment = 0;
    while(*p && isspace(*p)) {
        p++;
        i++;
    }
    prev_sep = 1;
    if (row->idx > 0 && editorRowHasOpenComment(&E.row[row->idx-1]))
        in_comment = 1;
    int oc = editorRowHasOpenComment(row);
    if (row->hl_oc != oc && row->idx+1 < E.numrows)
        editorUpdateSyntax(&E.row[row->idx+1]);
    row->hl_oc = oc;
}

void editorUpdateRow(erow *row) {
    unsigned int tabs = 0, nonprint = 0;
    int j, idx;
    free(row->render);
    for (j = 0; j < row->size; j++) if (row->chars[j] == TAB) tabs++;
    unsigned long long allocsize =
        (unsigned long long) row->size + tabs*8 + nonprint*9 + 1;
    if (allocsize > UINT32_MAX) {
        printf("Alguna línea del archivo editado es muy larga para el programa\n");
        exit(1);
    }
    row->render = malloc(row->size + tabs*8 + nonprint*9 + 1);
    idx = 0;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == TAB) {
            row->render[idx++] = ' ';
            while((idx+1) % 8 != 0) row->render[idx++] = ' ';
        } else row->render[idx++] = row->chars[j];
    row->rsize = idx;
    row->render[idx] = '\0';
    editorUpdateSyntax(row);
}

void editorInsertRow(int at, char *s, size_t len) {
    if (at > E.numrows) return;
    E.row = realloc(E.row,sizeof(erow)*(E.numrows+1));
    if (at != E.numrows) {
        memmove(E.row+at+1,E.row+at,sizeof(E.row[0])*(E.numrows-at));
        for (int j = at+1; j <= E.numrows; j++) E.row[j].idx++;
    }
    E.row[at].size = len;
    E.row[at].chars = malloc(len+1);
    memcpy(E.row[at].chars,s,len+1);
    E.row[at].hl = NULL;
    E.row[at].hl_oc = 0;
    E.row[at].render = NULL;
    E.row[at].rsize = 0;
    E.row[at].idx = at;
    editorUpdateRow(E.row+at);
    E.numrows++;
    E.dirty++;
}

void editorFreeRow(erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void editorDelRow(int at) {
    erow *row;
    if (at >= E.numrows) return;
    row = E.row+at;
    editorFreeRow(row);
    memmove(E.row+at,E.row+at+1,sizeof(E.row[0])*(E.numrows-at-1));
    for (int j = at; j < E.numrows-1; j++) E.row[j].idx++;
    E.numrows--;
    E.dirty++;
}

char *editorRowsToString(int *buflen) {
    char *buf = NULL, *p;
    int totlen = 0, j;
    for (j = 0; j < E.numrows; j++) totlen += E.row[j].size+1;
    *buflen = totlen;
    totlen++;
    p = buf = malloc(totlen);
    for (j = 0; j < E.numrows; j++) {
        memcpy(p,E.row[j].chars,E.row[j].size);
        p += E.row[j].size;
        *p = '\n';
        p++;
    }
    *p = '\0';
    return buf;
}

void editorRowInsertChar(erow *row, int at, int c) {
    if (at > row->size) {
        int padlen = at-row->size;
        row->chars = realloc(row->chars,row->size+padlen+2);
        memset(row->chars+row->size,' ',padlen);
        row->chars[row->size+padlen+1] = '\0';
        row->size += padlen+1;
    } else {
        row->chars = realloc(row->chars,row->size+2);
        memmove(row->chars+at+1,row->chars+at,row->size-at+1);
        row->size++;
    }
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars,row->size+len+1);
    memcpy(row->chars+row->size,s,len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
    if (row->size <= at) return;
    memmove(row->chars+at,row->chars+at+1,row->size-at);
    editorUpdateRow(row);
    row->size--;
    E.dirty++;
}

void editorInsertChar(int c) {
    int filerow = E.rowoff+E.cy;
    int filecol = E.coloff+E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    if (!row) while(E.numrows <= filerow) editorInsertRow(E.numrows,"",0);
    row = &E.row[filerow];
    editorRowInsertChar(row,filecol,c);
    if (E.cx == E.screencols-1) E.coloff++;
    else E.cx++;
    E.dirty++;
}

void editorInsertNewline() {
    int filerow = E.rowoff+E.cy;
    int filecol = E.coloff+E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    if (!row) {
        if (filerow == E.numrows) {
            editorInsertRow(filerow,"",0);
            goto fixcursor;
        }
        return;
    }
    if (filecol >= row->size) filecol = row->size;
    if (filecol == 0) editorInsertRow(filerow,"",0);
    else {
        editorInsertRow(filerow+1,row->chars+filecol,row->size-filecol);
        row = &E.row[filerow];
        row->chars[filecol] = '\0';
        row->size = filecol;
        editorUpdateRow(row);
    }
fixcursor:
    if (E.cy == E.screenrows-1) E.rowoff++;
    else E.cy++;
    E.cx = E.coloff = 0;
}

void editorDelChar() {
    int filerow = E.rowoff+E.cy;
    int filecol = E.coloff+E.cx;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    if (!row || (filecol == 0 && filerow == 0)) return;
    if (filecol == 0) {
        filecol = E.row[filerow-1].size;
        editorRowAppendString(&E.row[filerow-1],row->chars,row->size);
        editorDelRow(filerow);
        row = NULL;
        if (E.cy == 0) E.rowoff--;
        else E.cy--;
        E.cx = filecol;
        if (E.cx >= E.screencols) {
            int shift = (E.screencols-E.cx)+1;
            E.cx -= shift;
            E.coloff += shift;
        }
    } else {
        editorRowDelChar(row,filecol-1);
        if (E.cx == 0 && E.coloff) E.coloff--;
        else E.cx--;
    }
    if (row) editorUpdateRow(row);
    E.dirty++;
}

int editorOpen(char *filename) {
    FILE *fp;
    E.dirty = 0;
    free(E.filename);
    size_t fnlen = strlen(filename) + 1, linecap = 0;
    E.filename = malloc(fnlen);
    memcpy(E.filename,filename,fnlen);
    if (!(fp = fopen(filename,"r"))) {
        if (errno != ENOENT) {
            perror("Abriendo archivo");
            exit(1);
        }
        return 1;
    }
    char *line = NULL;
    ssize_t linelen;
    while((linelen = getline(&line,&linecap,fp)) != -1) {
        if (linelen && (line[linelen-1] == '\n' || line[linelen-1] == '\r'))
            line[--linelen] = '\0';
        editorInsertRow(E.numrows,line,linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
    return 0;
}

int editorSave() {
    int len, fd;
    char *buf = editorRowsToString(&len);
    if (!(fd = open(E.filename, O_RDWR | O_CREAT, 0644))) goto writeerr;
    if (ftruncate(fd,len) == -1) goto writeerr;
    if (write(fd,buf,len) != len) goto writeerr;
    close(fd);
    free(buf);
    E.dirty = 0;
    editorSetStatusMessage("%d bytes written on disk", len);
    return 0;
writeerr:
    free(buf);
    if (fd != -1) close(fd);
    editorSetStatusMessage("Can't save! I/O error: %s",strerror(errno));
    return 1;
}

void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b,ab->len+len);
    if (new == NULL) return;
    memcpy(new+ab->len,s,len);
    ab->b = new;
    ab->len += len;
}

void editorRefreshScreen() {
    int y;
    erow *r;
    char buf[32];
    struct abuf ab = {NULL, 0};
    abAppend(&ab,"\x1b[?25l",6);
    abAppend(&ab,"\x1b[H",3);
    for (y = 0; y < E.screenrows; y++) {
        int filerow = E.rowoff+y;
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.screenrows/3) {
                char welcome[80];
                int welcomelen = snprintf(welcome,sizeof(welcome),
                    "Editor de texto programa81 por PAF, CHG, MJPM y JJSE\x1b[0K\r\n");
                int padding = (E.screencols-welcomelen)/2;
                if (padding) {
                    abAppend(&ab,"~",1);
                    padding--;
                }
                while(padding--) abAppend(&ab," ",1);
                abAppend(&ab,welcome,welcomelen);
            } else abAppend(&ab,"~\x1b[0K\r\n",7);
            continue;
        }
        r = &E.row[filerow];
        int len = r->rsize - E.coloff;
        if (len > 0) {
            if (len > E.screencols) len = E.screencols;
            char *c = r->render+E.coloff;
            unsigned char *hl = r->hl+E.coloff;
            int j;
            for (j = 0; j < len; j++) abAppend(&ab,c+j,1);
        }
        abAppend(&ab,"\x1b[39m",5);
        abAppend(&ab,"\x1b[0K",4);
        abAppend(&ab,"\r\n",2);
    }
    abAppend(&ab,"\x1b[0K",4);
    abAppend(&ab,"\x1b[7m",4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
        E.filename, E.numrows, E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",E.rowoff+E.cy+1,E.numrows);
    if (len > E.screencols) len = E.screencols;
    abAppend(&ab,status,len);
    while(len < E.screencols) if (E.screencols - len == rlen) {
            abAppend(&ab,rstatus,rlen);
            break;
        } else {
            abAppend(&ab," ",1);
            len++;
        }
    abAppend(&ab,"\x1b[0m\r\n",6);
    abAppend(&ab,"\x1b[0K",4);
    int msglen = strlen(E.statusmsg);
    if (msglen && time(NULL)-E.statusmsg_time < 5)
        abAppend(&ab,E.statusmsg,msglen <= E.screencols ? msglen : E.screencols);
    int j, cx = 1, filerow = E.rowoff+E.cy;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    if (row)
        for (j = E.coloff; j < (E.cx+E.coloff); j++) {
            if (j < row->size && row->chars[j] == TAB) cx += 7-((cx)%8);
            cx++;
        }
    snprintf(buf,sizeof(buf),"\x1b[%d;%dH",E.cy+1,cx);
    abAppend(&ab,buf,strlen(buf));
    abAppend(&ab,"\x1b[?25h",6);
    write(STDOUT_FILENO,ab.b,ab.len);
    free(ab.b);
}

void editorMoveCursor(int key) {
    int filerow = E.rowoff+E.cy, filecol = E.coloff+E.cx, rowlen;
    erow *row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    switch(key) {
    case ARROW_LEFT:
        if (E.cx == 0){
            if (E.coloff) E.coloff--;
            else if (filerow > 0) {
                    E.cy--;
                    E.cx = E.row[filerow-1].size;
                    if (E.cx > E.screencols-1) {
                        E.coloff = E.cx-E.screencols+1;
                        E.cx = E.screencols-1;
                    }
                }
        } else E.cx -= 1;
        break;
    case ARROW_RIGHT:
        if (row && filecol < row->size) {
            if (E.cx == E.screencols-1) {
                E.coloff++;
            } else {
                E.cx += 1;
            }
        } else if (row && filecol == row->size) {
            E.cx = 0;
            E.coloff = 0;
            if (E.cy == E.screenrows-1) {
                E.rowoff++;
            } else {
                E.cy += 1;
            }
        }
        break;
    case ARROW_UP:
        if (E.cy == 0){
            if (E.rowoff) E.rowoff--;
        } else E.cy -= 1;
        break;
    case ARROW_DOWN:
        if (filerow < E.numrows){
            if (E.cy == E.screenrows-1) E.rowoff++;
            else E.cy += 1;
        }
        break;
    }
    filerow = E.rowoff+E.cy;
    filecol = E.coloff+E.cx;
    row = (filerow >= E.numrows) ? NULL : &E.row[filerow];
    rowlen = row ? row->size : 0;
    if (filecol > rowlen) {
        E.cx -= filecol-rowlen;
        if (E.cx < 0) {
            E.coloff += E.cx;
            E.cx = 0;
        }
    }
}

void editorProcessKeypress(int fd) {
    static int quit_times = EDITOR_QUIT_TIMES;
    int c = editorReadKey(fd);
    switch(c) {
        case ENTER:         
            editorInsertNewline();
            break;
        case CTRL_C: case CTRL_L: case ESC:
            break;
        case CTRL_Q:
            if (E.dirty && quit_times) {
                editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                    "Press Ctrl-Q %d more times to quit.", quit_times);
                quit_times--;
                return;
            }
            system("clear");
            exit(0);
            break;
        case CTRL_S:
            editorSave();
            break;
        case BACKSPACE: case CTRL_H: case DEL_KEY:
            editorDelChar();
            break;
        case PAGE_UP: case PAGE_DOWN:
            if (c == PAGE_UP && E.cy != 0)
                E.cy = 0;
            else if (c == PAGE_DOWN && E.cy != E.screenrows-1)
                E.cy = E.screenrows-1;
            int times = E.screenrows;
            while(times--) editorMoveCursor(c == PAGE_UP ? ARROW_UP: ARROW_DOWN);
            break;
        case ARROW_UP: case ARROW_DOWN: case ARROW_LEFT: case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
        default:
            editorInsertChar(c);
            break;
    }
    quit_times = EDITOR_QUIT_TIMES;
}

void updateWindowSize() {
    if (getWindowSize(STDIN_FILENO,STDOUT_FILENO,&E.screenrows,&E.screencols) == -1) {
        perror("Unable to query the screen for size (columns / rows)");
        exit(1);
    }
    E.screenrows -= 2;
}

void handleSigWinCh(int unused __attribute__((unused))) {
    updateWindowSize();
    if (E.cy > E.screenrows) E.cy = E.screenrows - 1;
    if (E.cx > E.screencols) E.cx = E.screencols - 1;
    editorRefreshScreen();
}

void initEditor() {
    E.cx = E.cy =  E.rowoff = E.coloff = E.numrows = E.dirty = 0;
    E.row = NULL;
    E.filename = NULL;
    updateWindowSize();
    signal(SIGWINCH, handleSigWinCh);
}

int main(int argc, char **argv) {
    initEditor();
    if (argc == 2) editorOpen(argv[1]);
    else if(argc > 2) {
        perror("Usage:\nprograma81\nprograma81 filename\n");
        exit(1);
    } else {
        char filename[80], command[94];
        printf("\nIngresa el nombre del archivo a crear (tamaño maximo 240): ");
        fflush(stdin);
        scanf("%s", filename);
        strcpy(command, "./programa81 ");
        strcat(command, filename);
        system(command);
        return 0;
    }
    enableRawMode(STDIN_FILENO);
    editorSetStatusMessage("COMANDOS: Ctrl-S = Guardar | Ctrl-Q = Cerrar");
    while(1) {
        editorRefreshScreen();
        editorProcessKeypress(STDIN_FILENO);
    }
    return 0;
}