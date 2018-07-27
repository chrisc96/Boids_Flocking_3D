#version 330 core

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

uniform float uAxisLength;

#ifdef _VERTEX_

flat out int v_instanceID;

void main() {
	v_instanceID = gl_InstanceID;
}

#endif

#ifdef _GEOMETRY_

layout(points) in;
layout(line_strip, max_vertices = 2) out;

flat in int v_instanceID[];

out vec3 v_color;

const vec3 points[] = vec3[](
	vec3(1, 0, 0),
	vec3(-1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, -1, 0),
	vec3(0, 0, 1),
	vec3(0, 0, -1)
);

void main() {
	v_color = abs(points[v_instanceID[0]]);
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(0.0, 0.0, 0.0, 1.0);
	EmitVertex();

	v_color = abs(points[v_instanceID[0]]);
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(points[v_instanceID[0]] * uAxisLength, 1.0);
	EmitVertex();
	EndPrimitive();
}

#endif

#ifdef _FRAGMENT_

in vec3 v_color;

out vec3 f_color;

void main() {
	f_color = v_color;
}

#endif