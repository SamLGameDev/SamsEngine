 #version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_height1;
    sampler2D texture_normal1;
    float shininess;
};

struct SpotLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

};

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;

    float constant;
    float linear;
    float quadratic;


};

struct DirLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

uniform Material material;
uniform SpotLight spotLight;
uniform DirLight directional;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

out vec4 FragColor;

in GS_OUT
{
   vec3 Normal;
   vec2 TexCoord; 
   vec3 FragPos;
} In;

uniform sampler2D ourTexture0; 
uniform float Visibility; 

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform mat4 View;

vec4 CalculateDirectionalLight(DirLight light, vec3 Normal, vec3 ViewDir);


vec4 CalculatePointLight(PointLight light, vec3 Normal, vec3 ViewDir, vec3 FragPos);

vec4 CalculateSpotLight(SpotLight light, vec3 Normal, vec3 ViewDir, vec3 FragPos);

void main()
{

   
   vec3 viewDir = normalize(-In.FragPos);

   vec3 norm = normalize(In.Normal);

   vec4 result = CalculateDirectionalLight(directional, norm, viewDir);

   for (int i = 0; i < NR_POINT_LIGHTS; i++)
   {
        if (pointLights[i].constant == 0)
        {
            continue;
        }
   	result += CalculatePointLight(pointLights[i], norm, viewDir, In.FragPos);
   }

   result += CalculateSpotLight(spotLight, norm, viewDir, In.FragPos);

   FragColor = result;
}

vec4 CalculateDirectionalLight(DirLight light, vec3 Normal, vec3 ViewDir)
{
      vec3 lightDir = normalize(-light.direction);

      float diff = max(dot(Normal, lightDir), 0.0);

      vec4 dTex = texture(material.texture_diffuse1, In.TexCoord).rgba;

      vec4 diffuse = vec4(light.diffuse * diff * vec3(dTex), dTex.a);

      vec3 reflectDir = normalize(lightDir + ViewDir);

      vec4 sTex = texture(material.texture_specular1, In.TexCoord).rgba;

      float spec = pow(max(dot(Normal, reflectDir), 0.0), material.shininess);
      vec4 specular = vec4(light.specular * spec * vec3(sTex), sTex.a);
      
      vec4 ambient = vec4(light.ambient * vec3(dTex), dTex.a);

      vec4 result = (ambient + diffuse + specular);
      
      return result;

}

vec4 CalculatePointLight(PointLight light, vec3 Normal, vec3 ViewDir, vec3 FragPos)
{
      vec3 fragToLight = light.position - FragPos;

      vec3 lightDir = normalize(fragToLight);

      float distance = length(fragToLight);

      float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
 
      float diff = max(dot(lightDir, Normal), 0.0);
      vec4 dTex = texture(material.texture_diffuse1, In.TexCoord).rgba;

      vec4 diffuse = vec4(light.diffuse * diff * vec3(dTex), dTex.a);



      vec3 reflectDir = normalize(lightDir + ViewDir);

      vec4 sTex = texture(material.texture_specular1, In.TexCoord).rgba;

      float spec = pow(max(dot(Normal, reflectDir), 0.0), material.shininess);
      vec4 specular = vec4(light.specular * spec * vec3(sTex), sTex.a);

      vec4 ambient = vec4(light.ambient * vec3(dTex), dTex.a);

      vec4 result = (ambient + diffuse + specular) * attenuation;

      return result;

}

vec4 CalculateSpotLight(SpotLight light, vec3 Normal, vec3 ViewDir, vec3 FragPos)
{
      vec3 fragToLight = light.position - FragPos;

      vec3 lightDir = normalize(fragToLight);

      float theta = dot(lightDir, normalize(-light.direction));

      float epsilon = light.innerCutOff - light.outerCutOff;

      float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

      float diff = max(dot(lightDir, Normal), 0.0);
      vec4 dTex = texture(material.texture_diffuse1, In.TexCoord).rgba;

      vec4 diffuse = vec4(light.diffuse * diff * vec3(dTex), dTex.a);



      diffuse *= intensity;

      vec3 reflectDir = normalize(lightDir + ViewDir);

      vec4 sTex = texture(material.texture_specular1, In.TexCoord).rgba;

      float spec = pow(max(dot(Normal, reflectDir), 0.0), material.shininess);
      vec4 specular = vec4(light.specular * spec * vec3(sTex), sTex.a);


      specular *= intensity;
      
      vec4 ambient = vec4(light.ambient * vec3(dTex), dTex.a);

      vec4 result = (ambient + diffuse + specular);

      return result;

}