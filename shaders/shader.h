#ifndef __SQUARE_H_
#define __SQUARE_H_

#include <string>

static const std::string VertexShaderSrc = R"(
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_normals;
//layout(location = 2) in vec2 a_texture; Incase we want to add textures to our model later.

//We specify our uniforms. We do not need to specify locations manually, but it helps with knowing what is bound where.
layout(location=0) uniform mat4 u_TransformationMat = mat4(1);
layout(location=1) uniform mat4 u_ViewMat           = mat4(1);
layout(location=2) uniform mat4 u_ProjectionMat     = mat4(1);
layout(location=3) uniform mat4 u_LightSpaceMat     = mat4(1);

out vec4 vertexPositions;
out vec3 normals;
out vec4 FragPosLightSpace;

void main()
{

//We need these in a different shader later down the pipeline, so we need to send them along. Can't just call in a_Position unfortunately.
vertexPositions = vec4(a_Position, 1.0);

//We also need them in Lightspace, so we compute that here
FragPosLightSpace = u_LightSpaceMat *  u_TransformationMat * vertexPositions;

//Find the correct values for our normals given that we move our object around in the world and the normals change quite a bit.
mat3 normalmatrix = transpose(inverse(mat3(u_ViewMat * u_TransformationMat)));

//Then normalize those new values so we do not accidentally go above length = 1. Also normalize the normals themselves beforehand, just to be sure calculations are accurate.
normals = normalize(normalmatrix * normalize(a_normals));

//We multiply our matrices with our position to change the positions of vertices to their final destinations.
gl_Position = u_ProjectionMat * u_ViewMat * u_TransformationMat * vertexPositions;
}
)";

static const std::string directionalLightFragmentShaderSrc = R"(
#version 430 core

in vec4 vertexPositions;
in vec3 normals;
in vec4 FragPosLightSpace;

out vec4 color;

uniform mat4 u_TransformationMat = mat4(1);
uniform mat4 u_ViewMat = mat4(1);

//We need the shadowmap texture to make sure we can utilize the values stored in it to compute lighting
uniform sampler2D u_ShadowMap;

uniform vec4 u_Color;
uniform vec3 u_LightColor;
uniform vec3 u_LightDirection;
uniform float u_Specularity;

//A new function for computing how shadowed a fragment is
float ShadowCalculation(
    in vec4 FragPosLightSpace,
    in vec3 normals
)
{
    //We don't need anything except these three values here. w might be needed for perspective projection, but is irrelevant here.
    vec3 projCoords = FragPosLightSpace.xyz;
    
    //projCoords are normally from -1 to 1, but we want them to be from 0 to 1
    projCoords = (projCoords * 0.5) +0.5;

    //We find the closest depthvalue for a point at the (x,y) coordinates of the vertex in the shadowmap
    //All our values are stored in the first parameter fo the shadowmap, so we only need .r
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    
    //Utilize a bias to make sure that floating point rounding does not cause artifacts
    float bias = max(0.05 * (1.0 - dot(normals, u_LightDirection)), 0.005);  
    
    //Check if the depth of the vertex is greater than the closest depth. 
    //If so, we are in shadow.
    float shadow;
    if(projCoords.z - bias > closestDepth)
    {
        shadow = 1.0;
    }
    else
    {
        shadow = 0.0;
    }

    //Make sure that if the coordinates are closer than the closest point of the shadowmap can be (1)
    //Then they are set to not be in shadow.
    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }   
    
    return shadow;
}



vec3 DirectionalLight(
    in vec3 color,
    in vec3 direction,
    in float shadow
)
{

    //Ambient lighting. Ambient light is light that is present even when normally no light should shine upon that part of the object. 
    //This is the poor mans way of simulating light reflecting from other surfaces in the room. For those that don't want to get into more advanced lighting models.
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * color;
    
    //Diffuse lighting. Light that scatters in all directions after hitting the object.
    vec3 dir_to_light = normalize(-direction);                                          //First we get the direction from the object to the lightsource ( which is of course the opposite of the direction of the light)
    vec3 diffuse = color * max(0.0, dot(normals, dir_to_light));                         //Then we find how strongly the light scatters in different directions, with a minimum of 0.0, via the normals and the direction we just found.

    //Specular Lighting. Light that is reflected away from the object it hits with the same angle it hit it. Think of it as light hitting a mirror and bouncing off, if you will.
    vec3 viewDirection = normalize(vec3(inverse(u_ViewMat) * vec4(0,0,0,1) - u_TransformationMat * vertexPositions)); //We find the direction from the surface the light hits to our camera.
    
    vec3 reflectionDirection = reflect(dir_to_light,normals);                                                         //And then we find the angle between the direction of the light and the direction from surface to camera
    
    float specular_power = pow(max(0.0,dot(viewDirection,reflectionDirection)),32);                                   //The closer together those two vectors are, the more powerful the specular light.
    vec3 specular = u_Specularity * specular_power * color;                                                           //Finally, multiply with how reflective the surface is and add the color.
    
    //We make sure to mutilply diffuse and specular with how shadowed our vertex is
    //The 1-shadow is not really necessary for this, but the values coming from the ShadowCalculation can be updated to give smoother transitions between shadows
    //In which case this might be usefull
    return ambient +(1.0-shadow) * (diffuse + specular);
}

void main()
{

//Calculate shadows
float shadow = ShadowCalculation(FragPosLightSpace,normals);

//Then send them to the lightfunction
vec3 light = DirectionalLight(u_LightColor,u_LightDirection,shadow);

//Finally, multiply with the color. Make sure the vector still has the same dimensions. Alpha channel is set to 1 here, because our object is not transparent. Might be different if you use a texture.
color = u_Color * vec4(light, 1.0);
}


)";

//Shaders which are only used for the debug-quad
static const std::string shadowVertexQuadShaderSrc = R"(
#version 430 core
layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texture; 

layout(location=0) uniform mat4 u_TransformationMat = mat4(1);
layout(location=1) uniform mat4 u_ViewMat           = mat4(1);
layout(location=2) uniform mat4 u_ProjectionMat     = mat4(1);

out vec2 TexCoords;
void main()
{
TexCoords = a_texture;
gl_Position = u_ProjectionMat * u_ViewMat * u_TransformationMat * vec4(a_position,1.0);
}

)";

static const std::string shadowFragQuadShaderSrc = R"(
#version 430 core
in vec2 TexCoords;

uniform sampler2D depthMap;

out vec4 color;

void main()
{
    float depthValue = texture(depthMap,TexCoords).r;
    color = vec4(vec3(depthValue),1.0);
}
)";

//Shaders that render the shadowmap
static const std::string shadowVertexShaderSrc = R"(
#version 430 core
layout(location = 0)in vec3 a_Position;

uniform mat4 u_TransformationMat = mat4(1);
uniform mat4 u_LightSpaceMat = mat4(1);

void main()
{
gl_Position = u_LightSpaceMat * u_TransformationMat * vec4(a_Position, 1.0);
}

)";

static const std::string shadowFragmentShaderSrc = R"(
#version 430 core
void main()
{
//We don't want any color output from these shaders, so we can have an empty fragment shader.
}
)";

#endif // __SQUARE_H_
