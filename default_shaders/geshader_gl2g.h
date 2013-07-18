#define smooth varying

#define ge_VertexTexcoord gl_MultiTexCoord0
#define ge_VertexColor gl_Color
#define ge_VertexNormal gl_Normal
#define ge_VertexPosition gl_Vertex

// Matrices
#define ge_MatrixProjection gl_ProjectionMatrix
#define ge_MatrixModelView gl_ModelViewMatrix
#define ge_MatrixNormal gl_NormalMatrix
#define ge_MatrixTexture gl_TextureMatrix
#define ge_ClipPlane gl_ClipPlane[0]

#define ge_Position gl_Position

uniform vec3 ge_CameraPosition;
uniform float ge_Time;
