
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;uniform bool usePhong;

void main()
{
	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;  
    vec3 norm = normalize(Normal);
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular;
    if(usePhong)
    {
       
        float specularStrength = 5;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular = specularStrength * spec * lightColor;    }    else
        {
            vec3 H = normalize(lightDir + viewDir);
            float specularStrength = 0.5;
           
            float spec = pow(max(dot(norm, H), 0.0), 32);
            specular = specularStrength * spec * lightColor;        }
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
        
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    outColor = (ambient + diffuse + specular) * objectColor;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}