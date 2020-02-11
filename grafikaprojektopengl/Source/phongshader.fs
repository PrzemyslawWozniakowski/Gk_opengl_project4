#version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 lightColor;
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 lightColor;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 4  

#define NR_SPOT_LIGHTS 3

in vec3 Normal;
in vec3 FragPos;
 
uniform vec3 objectColor;
uniform vec3 viewPos;uniform bool usePhong;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight[NR_SPOT_LIGHTS];
uniform int m;
uniform float specularStrength;
uniform float diffStrength;
uniform bool addFog;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usePhong, int m, float specularStrength, float diffStrength);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usePhong, int m, float specularStrength, float diffStrength);

void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

     vec3 result = vec3(0.0f,0.0f,0.0f);
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir, usePhong, m, specularStrength, diffStrength);    

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, usePhong, m, specularStrength, diffStrength);    
    
    result += lightColor*ambientStrength;
    result*=objectColor;

    if(addFog)
    {
        float mul =length(viewPos - FragPos)/30;
        result += vec3(1.0f,1.0f,1.0f)*mul; 
    }
    FragColor = vec4(result, 1.0);

}
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usePhong, int m, float specularStrength, float diffStrength)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    float spec;
    if(usePhong)
    {    
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), m);
    }
    else
        {
               
            vec3 H = normalize(lightDir + viewDir);           
            spec = pow(max(dot(norm, H), 0.0), m);
        }

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 diffuse = light.diffuse * diff * diffStrength;
    vec3 specular = light.specular * spec * specularStrength;
    diffuse *= attenuation;
    specular *= attenuation;
    return (diffuse + specular)*light.lightColor;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool usePhong, int m, float specularStrength, float diffStrength)
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    float spec;
    if(usePhong)
    {    
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), m);
    }
    else
        {
               
            vec3 H = normalize(lightDir + viewDir);           
            spec = pow(max(dot(norm, H), 0.0), m);
        }


    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 diffuse = light.diffuse * diff * diffStrength;
    vec3 specular = light.specular * spec * specularStrength;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return ( diffuse + specular)*light.lightColor;
}