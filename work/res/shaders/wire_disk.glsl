#version 330 core

uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;

uniform vec3 uColor;
uniform int uInstanceCount;

const float PI = 3.14159265359f;

#ifdef _VERTEX_

out int v_instanceID;

void main() {
	v_instanceID = gl_InstanceID;
}

#endif

#ifdef _GEOMETRY_

layout(points) in;
layout(line_strip, max_vertices = 2) out;

flat in int v_instanceID[];

void main() {

	float t0 = 2 * PI * float(v_instanceID[0]) / uInstanceCount;
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(cos(t0), sin(t0), 0, 1.0);
	EmitVertex();

	float t1 = 2 * PI * float(v_instanceID[0] + 1) / uInstanceCount;
	gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(cos(t1), sin(t1), 0, 1.0);
	EmitVertex();
	EndPrimitive();
}

#endif

#ifdef _FRAGMENT_

out vec3 f_color;

void main() {
	f_color = uColor;
}

#endif