#ifndef UNPROJECT_H_INCLUDED
#define UNPROJECT_H_INCLUDED

/* https://www.khronos.org/opengl/wiki/GluProject_and_gluUnProject_code */

extern int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate);

extern int glhUnProjectf(float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate);

extern void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2);

extern void MultiplyMatrixByVector4by4OpenGL_FLOAT(float *resultvector, const float *matrix, const float *pvector);

// This code comes directly from GLU except that it is for float
extern int glhInvertMatrixf2(float *m, float *out);

#endif /* UNPROJECT_H_INCLUDED */
