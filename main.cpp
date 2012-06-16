#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>




GLUquadric *quad;

GLuint texture[2];

GLint slices = 16;
GLint stacks = 16;

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image; //struktur data untuk


//ukuran image #bisa di set sesuai kebutuhan
#define checkImageWidth 164
#define checkImageHeight 164

GLubyte checkImage[checkImageWidth][checkImageHeight][3];


GLfloat angle = 0.0;

//mengambil gambar BMP

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size; // ukuran image dalam bytes
    unsigned long i; // standard counter.
    unsigned short int plane; // number of planes in image

    unsigned short int bpp; // jumlah bits per pixel
    char temp; // temporary color storage for var warna sementara untuk memastikan filenya ada


    if ((file = fopen(filename, "rb")) == NULL) {
        printf("File Not Found : %s\n", filename);
        return 0;
    }
    // mencari file header bmp
    fseek(file, 18, SEEK_CUR);
    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // membaca nilai height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    //printf("Height of %s: %lu\n", filename, image->sizeY);
    //menghitung ukuran image(asumsi 24 bits or 3 bytes per pixel).

    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&plane, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (plane != 1) {
        printf("Planes from %s is not 1: %u\n", filename, plane);
        return 0;
    }
    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);

        return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);
    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    for (i = 0; i < size; i += 3) { // membalikan semuan nilai warna (gbr - > rgb)
        temp = image->data[i];
        image->data[i] = image->data[i + 2];
        image->data[i + 2] = temp;
    }
    // we're done.
    return 1;
}

//mengambil tekstur

Image * loadTexture1() {
    Image *image1;
    // alokasi memmory untuk tekstur
    image1 = (Image *) malloc(sizeof (Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    //pic.bmp is a 64x64 picture
    if (!ImageLoad("images//rew.bmp", image1)) {
        exit(1);
    }
    return image1;
}

Image * loadTexture2() {
    Image *image1;
    // alokasi memmory untuk tekstur
    image1 = (Image *) malloc(sizeof (Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    //pic.bmp is a 64x64 picture
    if (!ImageLoad("images//rew.bmp", image1)) {
        exit(1);
    }
    return image1;
}

void init(void) {



    Image *image1 = loadTexture1();
    Image *image2 = loadTexture2();

    if (image1 == NULL) {
        printf("Image was not returned from loadTexture\n");
        exit(0);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate texture/ membuat texture
    glGenTextures(2, texture);

    //binding texture untuk membuat texture 2D
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    //menyesuaikan ukuran textur ketika image lebih besar dari texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //
    //menyesuaikan ukuran textur ketika image lebih kecil dari texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
            GL_UNSIGNED_BYTE, image1->data);

    //tekstur air

    //binding texture untuk membuat texture 2D
    glBindTexture(GL_TEXTURE_2D, texture[2]);

    //menyesuaikan ukuran textur ketika image lebih besar dari texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
    //menyesuaikan ukuran textur ketika image lebih kecil dari texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //

    glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB,
            GL_UNSIGNED_BYTE, image2->data);







    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_FLAT);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void display(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glPushMatrix();
    glRotatef(-angle, 0.0, 1.0, 0.0);

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(90, 0.0, 1.0, 0.0);
    glutSolidTeapot(2.0);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();
    angle = angle + 0.07;
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat) w / (GLfloat) h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("A basic OpenGL Window");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}