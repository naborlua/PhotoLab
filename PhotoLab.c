#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** global definitions ***/
#define SLEN 100 		/* maximum length of file names and string*/
#define SUCCESS 0		/* return code 0 for succeess */
#define EXIT 11			/* menu item number for EXIT */
#define MAX_PIXEL 255		/* max pixel value */
#define MIN_PIXEL 0		/* min pixel value */

#define NO_IMAGE_READ_MSG "No image is read yet!!\n"
const int WIDTH = 640;		/* image width */
const int HEIGHT = 480;		/* image height */
const int ZOOM_FACTOR = 2;	/* zooming factor for the zoom function */

/*** function declarations ***/

/* print a menu */
void PrintMenu();

/* read image from a file */
int ReadImage(char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* save a processed image */
int SaveImage(char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* reverse image color */
void Negative(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* color filter */
void ColorFilter(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], int target_r, int target_g, int target_b, int threshold, int replace_r, int replace_g, int replace_b);

/* edge detection */
void Edge(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* flip image horizontally */
void HFlip(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* mirror image vertically */
void VMirror(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* zoom an image */
void Zoom(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

/* add border */
void AddBorder(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], char color[SLEN], int border_width);

/* Test all functions */
void AutoTest(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]);

int main() {
    unsigned char   R[WIDTH][HEIGHT];	/* use three 2-D arrays to store R, G, B components */
    unsigned char   G[WIDTH][HEIGHT];
    unsigned char   B[WIDTH][HEIGHT];

    int option;			/* user input option */
    char fname[SLEN];		/* input file name */
    char colorOption[SLEN];

    PrintMenu();
    printf("Please make your choice: ");
    scanf("%d", &option);

    int rc = -1;			/* return code of ReadImage() */

    int target_r, target_g, target_b;	/* used by colorFiler() */
    double factor_r, factor_g, factor_b;
    int threshold;
    int border_width;		/* used by addBorder() */

    while (option != EXIT) {
        if (option == 1) {
            printf("Please input the file name to load: ");
            scanf("%s", fname);
            rc = ReadImage(fname, R, G, B);
        }
        /* menu item 2 - 9 requires image is loaded first */
        else if (option >= 2 && option <= 9) {
            if (rc != SUCCESS)	 {
                printf(NO_IMAGE_READ_MSG);
            }
            /* now image is loaded */
            else {
                switch(option) {
                    case 2:
                        printf("Please input the file name to save: ");
                        scanf("%s", fname);
                        SaveImage(fname, R, G, B);
                        break;
                    case 3:
                        Negative(R,G,B);
                        printf("\"Negative\" operation is done!\n");
                        break;
                    case 4:
                        printf("Enter Red   component for the target color: ");
                        scanf("%d", &target_r);
                        printf("Enter Green component for the target color: ");
                        scanf("%d", &target_g);
                        printf("Enter Blue  component for the target color: ");
                        scanf("%d", &target_b);
                        printf("Enter threshold for the color difference: ");
                        scanf("%d", &threshold);
                        printf("Enter value for Red component in the target color: ");
                        scanf("%lf", &factor_r);
                        printf("Enter value for Green component in the target color: ");
                        scanf("%lf", &factor_g);
                        printf("Enter value for Blue  component in the target color: ");
                        scanf("%lf", &factor_b);

                        ColorFilter(R, G, B, target_r, target_g, target_b, threshold, factor_r, factor_g, factor_b);
                        printf("\"Color Filter\" operation is done!\n");
                        break;
                    case 5:
                        Edge(R, G, B);
                        printf("\"Edge\" operation is done!\n");
                        break;
                    case 6:
                        HFlip(R,G,B);
                        printf( "\"Horizontally Flip\" operation is done!\n" );
                        break;
                    case 7:
                        VMirror(R, G, B);
                        printf("\"Vertically Mirror\" operation is done!\n");
                        break;
                    case 8:
                        printf("Enter border width:");
                        scanf("%d", &border_width);
                        printf("Available border colors : black, white, red, green, blue, yellow, cyan, pink, orange\n");
                        printf("Select border color from the options: ");
                        scanf("%s",colorOption);
                        AddBorder(R, G, B, colorOption, border_width);
                        printf("\"Border\" operation is done!\n");
                        break;
                    case 9:
                        Zoom(R, G, B);
                        printf("\"Zoom\" operation is done!\n");
                        break;
                    default:
                        break;
                }
            }
        }
        else if (option == 10) {
            AutoTest(R, G, B);
            rc = SUCCESS;	/* set returned code SUCCESS, since iamge is loaded */
        }
        else {
            printf("Invalid selection!\n");
        }

        /* Process finished, waiting for another input */
        PrintMenu();
        printf("Please make your choice: ");
        scanf("%d", &option);
    }
    printf("You exit the program.\n");
    return 0;
}

int ReadImage(char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    FILE           *File;
    char            Type[SLEN];
    int             Width, Height, MaxValue;
    int             x, y;
    char            ftype[] = ".ppm";
    char            fname_tmp[SLEN];      
    strcpy(fname_tmp, fname);
    strcat(fname_tmp, ftype);

    File = fopen(fname_tmp, "r");
    if (!File) {
        printf("Cannot open file \"%s\" for reading!\n", fname);
        return 1;
    }
    fscanf(File, "%79s", Type);
    if (Type[0] != 'P' || Type[1] != '6' || Type[2] != 0) {
        printf("\nUnsupported file format!\n");
        return 2;
    }
    fscanf(File, "%d", &Width);
    if (Width != WIDTH) {
        printf("\nUnsupported image width %d!\n", Width);
        return 3;
    }
    fscanf(File, "%d", &Height);
    if (Height != HEIGHT) {
        printf("\nUnsupported image height %d!\n", Height);
        return 4;
    }
    fscanf(File, "%d", &MaxValue);
    if (MaxValue != 255) {
        printf("\nUnsupported image maximum value %d!\n", MaxValue);
        return 5;
    }
    if ('\n' != fgetc(File)) {
        printf("\nCarriage return expected!\n");
        return 6;
    }
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++) {
            R[x][y] = fgetc(File);
            G[x][y] = fgetc(File);
            B[x][y] = fgetc(File);
        }
    if (ferror(File)) {
        printf("\nFile error while reading from file!\n");
        return 7;
    }
    printf("%s was read successfully!\n", fname_tmp);
    fclose(File);
    return 0;
}

int SaveImage(char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    FILE	*File;
    int	x, y;
    char	SysCmd[SLEN * 5];

    char	ftype[] = ".ppm";
    char	fname_tmp[SLEN];  /*avoid to modify on the original pointer, 11/10/10, X.Han*/
    char	fname_tmp2[SLEN];

    strcpy(fname_tmp, fname);
    strcpy(fname_tmp2, fname);
    strcat(fname_tmp2, ftype);

    File = fopen(fname_tmp2, "w");
    if (!File) {
        printf("Cannot open file \"%s\" for writing!\n", fname);
        return 1;
    }
    fprintf(File, "P6\n");
    fprintf(File, "%d %d\n", WIDTH, HEIGHT);
    fprintf(File, "255\n");

    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++) {
            fputc(R[x][y], File);
            fputc(G[x][y], File);
            fputc(B[x][y], File);
        }

    if (ferror(File)) {
        printf("\nFile error while writing to file!\n");
        return 2;
    }
    fclose(File);
    printf("%s was saved successfully. \n", fname_tmp2);

    /*
     * rename file to image.ppm, convert it to ~/public_html/<fname>.jpg
     * and make it world readable
     */
    sprintf(SysCmd, "/users/grad2/doemer/eecs22/bin/pnmtojpeg_hw2.tcsh %s", fname_tmp2);
    if (system(SysCmd) != 0) {
        printf("\nError while converting to JPG:\nCommand \"%s\" failed!\n", SysCmd);
        return 3;
    }
    printf("%s.jpg was stored for viewing. \n", fname_tmp);
    return (0);
}

/*******************************************/
/* Function implementations should go here */
/*******************************************/

/* Example of DIP */
/* age the image to make it look like old picture */
void Aging(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int x, y;
    for(y = 0; y < HEIGHT; y++ ) {
        for(x = 0; x < WIDTH; x++ ) {
            B[x][y] = (R[x][y] + G[x][y] + B[x][y]) / 5;
            R[x][y] = (unsigned char) (B[x][y] * 1.6);
            G[x][y] = (unsigned char) (B[x][y] * 1.6);
        }
    }
}

/* menu */
void PrintMenu() {
    printf("\n----------------------------\n");
    printf(" 1: Load a PPM image\n");
    printf(" 2: Save an image in PPM and JPEG format\n");
    printf(" 3: Make a negative of an image\n");
    printf(" 4: Color filter an image\n");
    printf(" 5: Sketch the edge of an image\n");
    printf(" 6: Flip an image horizontally\n");
    printf(" 7: Mirror an image vertically\n");
    printf(" 8: Add border to an image\n");
    printf(" 9: Zoom an image\n");
    printf("10: Test all functions\n");
    printf("11: Exit\n");
}

/* negative */
void Negative(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int x, y;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            R[x][y] = MAX_PIXEL - R[x][y];
            G[x][y] = MAX_PIXEL - G[x][y];
            B[x][y] = MAX_PIXEL - B[x][y];
        }
    }
}
/* colorFilter */
void ColorFilter (unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT],
        int target_r, int target_g, int target_b, int threshold, int replace_r, int replace_g, int replace_b) {
    int x, y;
    replace_r = (replace_r > MAX_PIXEL)? MAX_PIXEL : (replace_r < 0) ? 0 : replace_r;
    replace_r = (replace_r > MAX_PIXEL)? MAX_PIXEL : (replace_r < 0) ? 0 : replace_r;
    replace_r = (replace_r > MAX_PIXEL)? MAX_PIXEL : (replace_r < 0) ? 0 : replace_r;

    for (y = 0; y < HEIGHT; y++){
        for (x = 0; x < WIDTH; x++){
            if ((R[x][y] <= target_r + threshold) 
                    && (R[x][y] >= target_r - threshold) 
                    && (G[x][y] <= target_g + threshold) 
                    && (G[x][y] >= target_g - threshold)
                    && (B[x][y] <= target_b + threshold)
                    && (B[x][y] >= target_b - threshold)) {
                R[x][y] = replace_r;
                G[x][y] = replace_g;
                B[x][y] = replace_b;
            }
        }
    }
}

/* Edge */
void Edge(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int             x, y, m, n, a, b;
    int             tmpR = 0;
    int             tmpG = 0;
    int             tmpB = 0;
    unsigned char   R_tmp[WIDTH][HEIGHT];
    unsigned char   G_tmp[WIDTH][HEIGHT];
    unsigned char   B_tmp[WIDTH][HEIGHT];

    for (y = 0; y < HEIGHT; y++){
        for (x = 0; x < WIDTH; x++) {
            R_tmp[x][y] = R[x][y];
            G_tmp[x][y] = G[x][y];
            B_tmp[x][y] = B[x][y];
        }
    }

    for (y = 1; y < HEIGHT - 1; y++){
        for (x = 1; x < WIDTH - 1; x++){
            for (n = -1; n <= 1; n++){
                for (m = -1; m <= 1; m++) {
                    a = x + m;
                    b = y + n;
                    if (a > WIDTH - 1)
                        a = WIDTH - 1;
                    if (a < 0)
                        a = 0;
                    if (b > HEIGHT - 1)
                        b = HEIGHT - 1;
                    if (b < 0)
                        b = 0;
                    if ((n==0)&&(m==0)) {
                        tmpR += 8*R_tmp[a][b] ;
                        tmpG += 8*G_tmp[a][b] ;
                        tmpB += 8*B_tmp[a][b] ;
                    }
                    else {
                        tmpR -= R_tmp[a][b] ;
                        tmpG -= G_tmp[a][b] ;
                        tmpB -= B_tmp[a][b] ;
                    }
                }
            }
            R[x][y] = (tmpR>MAX_PIXEL)? MAX_PIXEL: (tmpR<0)? 0: tmpR ;
            G[x][y] = (tmpG>MAX_PIXEL)? MAX_PIXEL: (tmpG<0)? 0: tmpG ;
            B[x][y] = (tmpB>MAX_PIXEL)? MAX_PIXEL: (tmpB<0)? 0: tmpB ;
            tmpR = tmpG = tmpB = 0;
        }
    }
    for (y = 0; y < HEIGHT; y++) {
        R[0][y] = 0;
        G[0][y] = 0;
        B[0][y] = 0;
        R[WIDTH - 1][y] = 0;
        G[WIDTH - 1][y] = 0;
        B[WIDTH - 1][y] = 0;
    }
    for (x = 0; x < WIDTH; x++) {
        R[x][0] = 0;
        G[x][0] = 0;
        B[x][0] = 0;
        R[x][HEIGHT - 1] = 0;
        G[x][HEIGHT - 1] = 0;
        B[x][HEIGHT - 1] = 0;
    }
}

/* hFlip */
void HFlip(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int x, y;
    for (y = 0; y < HEIGHT; y ++) {
        for (x = 0; x < WIDTH / 2; x ++) {
            unsigned char r = R[WIDTH - 1 - x][y];
            unsigned char g = G[WIDTH - 1 - x][y];
            unsigned char b = B[WIDTH - 1 - x][y];
            R[WIDTH - 1 - x][y] = R[x][y];
            G[WIDTH - 1 - x][y] = G[x][y];
            B[WIDTH - 1 - x][y] = B[x][y];
            R[x][y] = r;
            G[x][y] = g;
            B[x][y] = b;
        }
    }
}

/* vMirror */
void VMirror(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int x, y;
    for (y = 0; y < HEIGHT / 2; y ++) {
        for (x = 0; x < WIDTH; x ++) {
            R[x][HEIGHT - 1 - y] = R[x][y];
            G[x][HEIGHT - 1 - y] = G[x][y];
            B[x][HEIGHT - 1 - y] = B[x][y];
        }
    }
}

/* zoom */
void Zoom(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    int x, y;
    const int X_OFFSET = WIDTH / 4;
    const int Y_OFFSET = HEIGHT / 4;
    unsigned char tmpR[WIDTH][HEIGHT];
    unsigned char tmpG[WIDTH][HEIGHT];
    unsigned char tmpB[WIDTH][HEIGHT];
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            tmpR[x][y] = R[x][y];
            tmpG[x][y] = G[x][y];
            tmpB[x][y] = B[x][y];
        }
    }
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            R[x][y] = tmpR[x / ZOOM_FACTOR + X_OFFSET][y / ZOOM_FACTOR + Y_OFFSET];
            G[x][y] = tmpG[x / ZOOM_FACTOR + X_OFFSET][y / ZOOM_FACTOR + Y_OFFSET];
            B[x][y] = tmpB[x / ZOOM_FACTOR + X_OFFSET][y / ZOOM_FACTOR + Y_OFFSET];
        }
    }
}

/* AddBorder */
void AddBorder(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT],
        char color[SLEN], int border_width) {
    int x, y;
    int border_r = 255;
    int border_g = 255;
    int border_b = 255;
    if (!strcmp(color, "black")) {
        border_r = 0;
        border_g = 0;
        border_b = 0;
    } else if (!strcmp(color, "white")) {
        border_r = 255;
        border_g = 255;
        border_b = 255;
    } else if (!strcmp(color, "red")) {
        border_r = 255;
        border_g = 0;
        border_b = 0;
    } else if (!strcmp(color, "green")) {
        border_r = 0;
        border_g = 255;
        border_b = 0;
    } else if (!strcmp(color, "blue")) {
        border_r = 0;
        border_g = 0;
        border_b = 255;
    } else if (!strcmp(color, "yellow")) {
        border_r = 255;
        border_g = 255;
        border_b = 0;
    } else if (!strcmp(color, "cyan")) {
        border_r = 0;
        border_g = 255;
        border_b = 255;
    } else if (!strcmp(color, "pink")) {
        border_r = 255;
        border_g = 192;
        border_b = 203;
    } else if (!strcmp(color, "orange")) {
        border_r = 255;
        border_g = 165;
        border_b = 0;
    }
    const int X_BORDER_WIDTH = 9 * border_width / 16;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if ((y < border_width) || (y > HEIGHT-1 - border_width) ||
                    (x < X_BORDER_WIDTH) || (x > WIDTH - 1 - X_BORDER_WIDTH)) {
                R[x][y] = border_r;
                G[x][y] = border_g;
                B[x][y] = border_b;
            }
        }
    }
}
/* test all DIP operations */
void AutoTest(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) {
    char fname[SLEN] = "EH";
    char sname[SLEN];

    ReadImage(fname, R, G, B);
    Negative (R, G, B);
    strcpy(sname, "negative");
    SaveImage(sname, R, G, B);
    printf("Negative tested!\n\n");

    ReadImage(fname, R, G, B);
    ColorFilter(R, G, B, 190, 100, 150, 60, 0, 0, 255);
    strcpy(sname, "colorfilter");
    SaveImage(sname, R, G, B);
    printf("Color Filter tested!\n\n");

    ReadImage(fname, R, G, B);
    Edge(R, G, B);
    strcpy(sname, "edge");
    SaveImage(sname, R, G, B);
    printf("Edge Detection tested!\n\n");

    ReadImage(fname, R, G, B);
    HFlip(R, G, B);
    strcpy(sname, "hflip");
    SaveImage(sname, R, G, B);
    printf("HFlip tested!\n\n");

    ReadImage(fname, R, G, B);
    VMirror(R, G, B);
    strcpy(sname, "vmirror");
    SaveImage(sname, R, G, B);
    printf("VMirror tested!\n\n");

    ReadImage(fname, R, G, B);
    AddBorder (R, G, B, "yellow", 32);
    strcpy(sname, "border");
    SaveImage(sname, R, G, B);
    printf("Border tested!\n\n");

    ReadImage(fname, R, G, B);
    Zoom (R, G, B);
    strcpy(sname, "zoom");
    SaveImage(sname, R, G, B);
    printf("Zoom tested!\n\n");
}
