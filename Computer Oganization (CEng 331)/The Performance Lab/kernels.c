/********
 * Kernels to be optimized for the CS:APP Performance Lab
 ********/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
/*
 * Please fill in the following team_t struct
 */
team_t team = {
        "homeboiz",    /* Team Name */

        "e258793",      /* First student ID */
        "Berkay Özkan",       /* First student name */

        "e244417",             /* Second student ID */
        "Denizcan Yılmaz",           /* Second student name */

        "e244822",             /* Third student ID */
        "Ozan Cinci"            /* Third student Name */
};


/****
 * CONVOLUTION KERNEL
 ****/

/*******
 * Your different versions of the convolution functions  go here
 *******/

/*
 * naive_conv - The naive baseline version of convolution
 */
char naive_conv_descr[] = "naive_conv: Naive baseline implementation";
void naive_conv(int dim, pixel *src, pixel *ker, unsigned *dst) {
    int i,j,k,l;

    for(i = 0; i < dim-8+1; i++)
        for(j = 0; j < dim-8+1; j++) {
            dst[RIDX(i, j, dim)] = 0;
            for(k = 0; k < 8; k++)
                for(l = 0; l < 8; l++) {
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].red * ker[RIDX(k, l, 8)].red;
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].green * ker[RIDX(k, l, 8)].green;
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].blue * ker[RIDX(k, l, 8)].blue;
                    
                }

        }
}

/* --------------------------------------------------------------------------------- */
char convolution_descr[] = "Convolution: Current working version";
void convolution(int dim, pixel *src, pixel *ker, unsigned *dst)
{
    int i, j;
    unsigned calcDim = dim - 8, summationResult;
    pixel* srcCopy;
    pixel kernValues[64];
    src += calcDim * (dim + 1);
    dst += calcDim * (dim + 1);
    memcpy(kernValues, ker, 64 * sizeof(pixel));
    
    for (i = calcDim; i >= 0; i--) {
        srcCopy = src;
        for (j = calcDim; j >= 0; j--) {
            summationResult = (srcCopy[0].red) * (kernValues[0].red);
            summationResult = summationResult + (srcCopy[0].green) * (kernValues[0].green) + (srcCopy[0].blue) * (kernValues[0].blue) +
                                + (srcCopy[1].red) * (kernValues[1].red) + (srcCopy[1].green) * (kernValues[1].green) + (srcCopy[1].blue) * (kernValues[1].blue) 
                                + (srcCopy[2].red) * (kernValues[2].red) + (srcCopy[2].green) * (kernValues[2].green) + (srcCopy[2].blue) * (kernValues[2].blue)
                                + (srcCopy[3].red) * (kernValues[3].red) + (srcCopy[3].green) * (kernValues[3].green) + (srcCopy[3].blue) * (kernValues[3].blue)
                                + (srcCopy[4].red) * (kernValues[4].red) + (srcCopy[4].green) * (kernValues[4].green) + (srcCopy[4].blue) * (kernValues[4].blue)
                                + (srcCopy[5].red) * (kernValues[5].red) + (srcCopy[5].green) * (kernValues[5].green) + (srcCopy[5].blue) * (kernValues[5].blue)
                                + (srcCopy[6].red) * (kernValues[6].red) + (srcCopy[6].green) * (kernValues[6].green) + (srcCopy[6].blue) * (kernValues[6].blue)
                                + (srcCopy[7].red) * (kernValues[7].red) + (srcCopy[7].green) * (kernValues[7].green) + (srcCopy[7].blue) * (kernValues[7].blue);

            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[8].red) + (srcCopy[0].green) * (kernValues[8].green) + (srcCopy[0].blue) * (kernValues[8].blue) +
                                + (srcCopy[1].red) * (kernValues[9].red) + (srcCopy[1].green) * (kernValues[9].green) + (srcCopy[1].blue) * (kernValues[9].blue) 
                                + (srcCopy[2].red) * (kernValues[10].red) + (srcCopy[2].green) * (kernValues[10].green) + (srcCopy[2].blue) * (kernValues[10].blue)
                                + (srcCopy[3].red) * (kernValues[11].red) + (srcCopy[3].green) * (kernValues[11].green) + (srcCopy[3].blue) * (kernValues[11].blue)
                                + (srcCopy[4].red) * (kernValues[12].red) + (srcCopy[4].green) * (kernValues[12].green) + (srcCopy[4].blue) * (kernValues[12].blue)
                                + (srcCopy[5].red) * (kernValues[13].red) + (srcCopy[5].green) * (kernValues[13].green) + (srcCopy[5].blue) * (kernValues[13].blue)
                                + (srcCopy[6].red) * (kernValues[14].red) + (srcCopy[6].green) * (kernValues[14].green) + (srcCopy[6].blue) * (kernValues[14].blue)
                                + (srcCopy[7].red) * (kernValues[15].red) + (srcCopy[7].green) * (kernValues[15].green) + (srcCopy[7].blue) * (kernValues[15].blue);


            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[16].red) + (srcCopy[0].green) * (kernValues[16].green) + (srcCopy[0].blue) * (kernValues[16].blue) +
                                + (srcCopy[1].red) * (kernValues[17].red) + (srcCopy[1].green) * (kernValues[17].green) + (srcCopy[1].blue) * (kernValues[17].blue) 
                                + (srcCopy[2].red) * (kernValues[18].red) + (srcCopy[2].green) * (kernValues[18].green) + (srcCopy[2].blue) * (kernValues[18].blue)
                                + (srcCopy[3].red) * (kernValues[19].red) + (srcCopy[3].green) * (kernValues[19].green) + (srcCopy[3].blue) * (kernValues[19].blue)
                                + (srcCopy[4].red) * (kernValues[20].red) + (srcCopy[4].green) * (kernValues[20].green) + (srcCopy[4].blue) * (kernValues[20].blue)
                                + (srcCopy[5].red) * (kernValues[21].red) + (srcCopy[5].green) * (kernValues[21].green) + (srcCopy[5].blue) * (kernValues[21].blue)
                                + (srcCopy[6].red) * (kernValues[22].red) + (srcCopy[6].green) * (kernValues[22].green) + (srcCopy[6].blue) * (kernValues[22].blue)
                                + (srcCopy[7].red) * (kernValues[23].red) + (srcCopy[7].green) * (kernValues[23].green) + (srcCopy[7].blue) * (kernValues[23].blue);

            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[24].red) + (srcCopy[0].green) * (kernValues[24].green) + (srcCopy[0].blue) * (kernValues[24].blue) +
                                + (srcCopy[1].red) * (kernValues[25].red) + (srcCopy[1].green) * (kernValues[25].green) + (srcCopy[1].blue) * (kernValues[25].blue) 
                                + (srcCopy[2].red) * (kernValues[26].red) + (srcCopy[2].green) * (kernValues[26].green) + (srcCopy[2].blue) * (kernValues[26].blue)
                                + (srcCopy[3].red) * (kernValues[27].red) + (srcCopy[3].green) * (kernValues[27].green) + (srcCopy[3].blue) * (kernValues[27].blue)
                                + (srcCopy[4].red) * (kernValues[28].red) + (srcCopy[4].green) * (kernValues[28].green) + (srcCopy[4].blue) * (kernValues[28].blue)
                                + (srcCopy[5].red) * (kernValues[29].red) + (srcCopy[5].green) * (kernValues[29].green) + (srcCopy[5].blue) * (kernValues[29].blue)
                                + (srcCopy[6].red) * (kernValues[30].red) + (srcCopy[6].green) * (kernValues[30].green) + (srcCopy[6].blue) * (kernValues[30].blue)
                                + (srcCopy[7].red) * (kernValues[31].red) + (srcCopy[7].green) * (kernValues[31].green) + (srcCopy[7].blue) * (kernValues[31].blue);

            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[32].red) + (srcCopy[0].green) * (kernValues[32].green) + (srcCopy[0].blue) * (kernValues[32].blue) +
                                + (srcCopy[1].red) * (kernValues[33].red) + (srcCopy[1].green) * (kernValues[33].green) + (srcCopy[1].blue) * (kernValues[33].blue) 
                                + (srcCopy[2].red) * (kernValues[34].red) + (srcCopy[2].green) * (kernValues[34].green) + (srcCopy[2].blue) * (kernValues[34].blue)
                                + (srcCopy[3].red) * (kernValues[35].red) + (srcCopy[3].green) * (kernValues[35].green) + (srcCopy[3].blue) * (kernValues[35].blue)
                                + (srcCopy[4].red) * (kernValues[36].red) + (srcCopy[4].green) * (kernValues[36].green) + (srcCopy[4].blue) * (kernValues[36].blue)
                                + (srcCopy[5].red) * (kernValues[37].red) + (srcCopy[5].green) * (kernValues[37].green) + (srcCopy[5].blue) * (kernValues[37].blue)
                                + (srcCopy[6].red) * (kernValues[38].red) + (srcCopy[6].green) * (kernValues[38].green) + (srcCopy[6].blue) * (kernValues[38].blue)
                                + (srcCopy[7].red) * (kernValues[39].red) + (srcCopy[7].green) * (kernValues[39].green) + (srcCopy[7].blue) * (kernValues[39].blue);

            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[40].red) + (srcCopy[0].green) * (kernValues[40].green) + (srcCopy[0].blue) * (kernValues[40].blue) +
                                + (srcCopy[1].red) * (kernValues[41].red) + (srcCopy[1].green) * (kernValues[41].green) + (srcCopy[1].blue) * (kernValues[41].blue) 
                                + (srcCopy[2].red) * (kernValues[42].red) + (srcCopy[2].green) * (kernValues[42].green) + (srcCopy[2].blue) * (kernValues[42].blue)
                                + (srcCopy[3].red) * (kernValues[43].red) + (srcCopy[3].green) * (kernValues[43].green) + (srcCopy[3].blue) * (kernValues[43].blue)
                                + (srcCopy[4].red) * (kernValues[44].red) + (srcCopy[4].green) * (kernValues[44].green) + (srcCopy[4].blue) * (kernValues[44].blue)
                                + (srcCopy[5].red) * (kernValues[45].red) + (srcCopy[5].green) * (kernValues[45].green) + (srcCopy[5].blue) * (kernValues[45].blue)
                                + (srcCopy[6].red) * (kernValues[46].red) + (srcCopy[6].green) * (kernValues[46].green) + (srcCopy[6].blue) * (kernValues[46].blue)
                                + (srcCopy[7].red) * (kernValues[47].red) + (srcCopy[7].green) * (kernValues[47].green) + (srcCopy[7].blue) * (kernValues[47].blue);


            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[48].red) + (srcCopy[0].green) * (kernValues[48].green) + (srcCopy[0].blue) * (kernValues[48].blue) +
                                + (srcCopy[1].red) * (kernValues[49].red) + (srcCopy[1].green) * (kernValues[49].green) + (srcCopy[1].blue) * (kernValues[49].blue) 
                                + (srcCopy[2].red) * (kernValues[50].red) + (srcCopy[2].green) * (kernValues[50].green) + (srcCopy[2].blue) * (kernValues[50].blue)
                                + (srcCopy[3].red) * (kernValues[51].red) + (srcCopy[3].green) * (kernValues[51].green) + (srcCopy[3].blue) * (kernValues[51].blue)
                                + (srcCopy[4].red) * (kernValues[52].red) + (srcCopy[4].green) * (kernValues[52].green) + (srcCopy[4].blue) * (kernValues[52].blue)
                                + (srcCopy[5].red) * (kernValues[53].red) + (srcCopy[5].green) * (kernValues[53].green) + (srcCopy[5].blue) * (kernValues[53].blue)
                                + (srcCopy[6].red) * (kernValues[54].red) + (srcCopy[6].green) * (kernValues[54].green) + (srcCopy[6].blue) * (kernValues[54].blue)
                                + (srcCopy[7].red) * (kernValues[55].red) + (srcCopy[7].green) * (kernValues[55].green) + (srcCopy[7].blue) * (kernValues[55].blue);


            srcCopy += dim;
            summationResult = summationResult + (srcCopy[0].red) * (kernValues[56].red) + (srcCopy[0].green) * (kernValues[56].green) + (srcCopy[0].blue) * (kernValues[56].blue) +
                                + (srcCopy[1].red) * (kernValues[57].red) + (srcCopy[1].green) * (kernValues[57].green) + (srcCopy[1].blue) * (kernValues[57].blue) 
                                + (srcCopy[2].red) * (kernValues[58].red) + (srcCopy[2].green) * (kernValues[58].green) + (srcCopy[2].blue) * (kernValues[58].blue)
                                + (srcCopy[3].red) * (kernValues[59].red) + (srcCopy[3].green) * (kernValues[59].green) + (srcCopy[3].blue) * (kernValues[59].blue)
                                + (srcCopy[4].red) * (kernValues[60].red) + (srcCopy[4].green) * (kernValues[60].green) + (srcCopy[4].blue) * (kernValues[60].blue)
                                + (srcCopy[5].red) * (kernValues[61].red) + (srcCopy[5].green) * (kernValues[61].green) + (srcCopy[5].blue) * (kernValues[61].blue)
                                + (srcCopy[6].red) * (kernValues[62].red) + (srcCopy[6].green) * (kernValues[62].green) + (srcCopy[6].blue) * (kernValues[62].blue)
                                + (srcCopy[7].red) * (kernValues[63].red) + (srcCopy[7].green) * (kernValues[63].green) + (srcCopy[7].blue) * (kernValues[63].blue);

            *dst = summationResult;
            srcCopy = --src;
            --dst;
        }
        src -= 7;
        dst -= 7;
    }
}

/*********
 * register_conv_functions - Register all of your different versions
 *     of the convolution functions  with the driver by calling the
 *     add_conv_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********/

void register_conv_functions() {
    add_conv_function(&naive_conv, naive_conv_descr);
    add_conv_function(&convolution, convolution_descr);
    /* ... Register additional test functions here */
}




/****
 * AVERAGE POOLING KERNEL
 ****/

/*******
 * Your different versions of the average pooling  go here
 *******/

/*
 * naive_average_pooling - The naive baseline version of average pooling
 */
char naive_average_pooling_descr[] = "Naive Average Pooling: Naive baseline implementation";
void naive_average_pooling(int dim, pixel *src, pixel *dst) {
    int i,j,k,l;

    for(i = 0; i < dim/2; i++)
        for(j = 0; j < dim/2; j++) {
            dst[RIDX(i, j, dim/2)].red = 0;
            dst[RIDX(i, j, dim/2)].green = 0;
            dst[RIDX(i, j, dim/2)].blue = 0;
            for(k = 0; k < 2; k++) {
                for (l = 0; l < 2; l++) {
                    dst[RIDX(i, j, dim/2)].red += src[RIDX(i*2 + k, j*2 + l, dim)].red;
                    dst[RIDX(i, j, dim/2)].green += src[RIDX(i*2 + k, j*2 + l, dim)].green;
                    dst[RIDX(i, j, dim/2)].blue += src[RIDX(i*2 + k, j*2 + l, dim)].blue;
                }
            }
            dst[RIDX(i, j, dim/2)].red /= 4;
            dst[RIDX(i, j, dim/2)].green /= 4;
            dst[RIDX(i, j, dim/2)].blue /= 4;
        }
}

char average_pooling_descr[] = "Average Pooling: Current working version";
void average_pooling(int dim, pixel *src, pixel *dst)
{
    int i,j, i2, j2;
    int dimOver2 = dim/2;
    pixel *temp, *tmp;
    int sumRed, sumBlue, sumGreen;

    for(i = 0; i < dimOver2; i++){
        i2 = i << 1;
        for(j = 0; j < dimOver2; j++) {
            j2 = j << 1;

            //k l
            //0  0
            temp = src + (i2 * dim + j2) ;
            sumRed = temp->red;
            sumGreen = temp->green;
            sumBlue = temp->blue;
            //0  1
            temp += 1 ;
            sumRed += temp->red;
            sumGreen += temp->green;
            sumBlue+= temp->blue;
            //1  0
            temp += dim -1 ;
            sumRed += temp->red;
            sumGreen += temp->green;
            sumBlue+= temp->blue;
            //1  1
            temp += 1; 
            sumRed += temp->red;
            sumGreen += temp->green;
            sumBlue+= temp->blue;

            tmp = dst + (i * dimOver2 + j);
            tmp->red = sumRed >> 2 ;
            tmp->green = sumGreen >> 2 ;
            tmp->blue = sumBlue >> 2 ;
        }
    }
}

/**********
 * register_average_pooling_functions - Register all of your different versions
 *     of the average pooling  with the driver by calling the
 *     add_average_pooling_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 **********/

void register_average_pooling_functions() {
    add_average_pooling_function(&naive_average_pooling, naive_average_pooling_descr);
    add_average_pooling_function(&average_pooling, average_pooling_descr);
    /* ... Register additional test functions here */
}
