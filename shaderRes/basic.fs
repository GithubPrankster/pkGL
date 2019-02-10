#version 330 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
};

in vec3 fragPos;  
in vec3 normal;  
in vec2 texCoord;

uniform vec3 viewPos;
uniform Light light;

uniform sampler2D diffuse;
uniform samplerCube shadowMap;

uniform float far_plane;

float shadowCalculate(vec3 fragPos)
{
    vec3 fragLight = fragPos - light.position;
  
    float closestDepth = texture(shadowMap, fragLight).r;
    closestDepth *= far_plane;

    float currentDepth = length(fragLight);

    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        

    return shadow;
}

void main(){           
    vec3 color = texture(diffuse, texCoord).rgb;
    // ambient
    vec3 ambient = light.ambient * color;
    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 normal = normalize(normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * light.ambient;    
    // calculate shadow
	float shadow = shadowCalculate(fragPos);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;  
    FragColor = vec4(lighting, 1.0);
}