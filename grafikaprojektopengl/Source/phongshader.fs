#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
 
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;uniform bool usePhong;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular;
    if(usePhong)
    {
       
        float specularStrength = 0.9;
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

        vec3 result = (ambient + diffuse + specular) * objectColor;
        FragColor = vec4(result, 1.0);
}