#version 330 core

in vec3 vNormal;
out vec4 FragColor;

void main() {
    vec3 norm = normalize(vNormal);
    
    // Simple directional light pointing down and slightly forward
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5)); 
    
    // Calculate diffuse lighting with a baseline ambient floor of 0.3
    float diff = max(dot(norm, lightDir), 0.3); 
    

    vec3 baseColor = vec3(.31, .2, .06); 
    
    FragColor = vec4(baseColor * diff, 1.0);
}