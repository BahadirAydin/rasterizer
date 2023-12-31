#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include "Camera.h"
#include "Matrix4.h"
#include "Mesh.h"
#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"
#include "Clipping.h"

#include "Utils.h"
#include "MeshTransformations.h"
#include "TriangleTransformations.h"

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		this->vertices.push_back(vertex);
		this->colorsOfVertices.push_back(color);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	command = "magick convert " + ppmFileName + " ./outputs/" + ppmFileName + ".png";
	system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera)
{
    Matrix4 cameraTransformationMatrix = Utils::worldToCameraMatrix(camera);
    double l = camera->left;
    double r = camera->right;
    double b = camera->bottom;
    double t = camera->top;
    double n = camera->near;
    double f = camera->far;
    Matrix4 projectionTransformationMatrix;
    if(camera->projectionType == PERSPECTIVE_PROJECTION){
        projectionTransformationMatrix = Utils::perspectiveProjectionMatrix(l, r, b, t, n, f);
    }
    else if (camera->projectionType == ORTOGRAPHIC_PROJECTION){
        projectionTransformationMatrix = Utils::orthographicProjectionMatrix(l, r, b, t, n, f);
    }
    Matrix4 viewportTransformationMatrix = Utils::viewportMatrix(camera->horRes, camera->verRes);
    Matrix4 proj_mult_camera = multiplyMatrixWithMatrix(projectionTransformationMatrix, cameraTransformationMatrix);

    for(Mesh *m : this -> meshes)
    {
        Matrix4 meshTransformationMatrix = MeshTransformations::applyAllTransformations(m->numberOfTransformations, m->transformationTypes, m->transformationIds, this->translations, this->scalings, this->rotations);
        Matrix4 final = multiplyMatrixWithMatrix(proj_mult_camera, meshTransformationMatrix);

        for(Triangle &t : m->triangles)
        {
            const Vec3 *v0 = this->vertices[t.vertexIds[0]-1];
            const Vec3 *v1 = this->vertices[t.vertexIds[1]-1];
            const Vec3 *v2 = this->vertices[t.vertexIds[2]-1];
            const Color *c0 = this->colorsOfVertices[v0->colorId - 1];
            const Color *c1 = this->colorsOfVertices[v1->colorId - 1];
            const Color *c2 = this->colorsOfVertices[v2->colorId - 1];
            std::vector<Vec4> transformed_vertices = TriangleTransformations::transformTriangle(t, final , this->vertices);


            if(this->cullingEnabled)
            {
                // check if triangle is backfacing
                const Vec3 new_v0 = {transformed_vertices[0].x, transformed_vertices[0].y, transformed_vertices[0].z, v0 -> colorId}; 
                const Vec3 new_v1 = {transformed_vertices[1].x, transformed_vertices[1].y, transformed_vertices[1].z, v1 -> colorId};
                const Vec3 new_v2 = {transformed_vertices[2].x, transformed_vertices[2].y, transformed_vertices[2].z, v2 -> colorId};

                const Vec3 v0v1 = subtractVec3(new_v1, new_v0);
                const Vec3 v0v2 = subtractVec3(new_v2, new_v0);
                const Vec3 cp = crossProductVec3(v0v1, v0v2);
                const Vec3 normal = normalizeVec3(cp);
                if (dotProductVec3(normal, new_v0) < 0) {
                    continue;
                }
            }

            if(m->type == SOLID_MESH)
            {
                const Vec4 p_v0 = multiplyMatrixWithVec4(viewportTransformationMatrix, Vec4({
                    transformed_vertices[0].x / transformed_vertices[0].t,
                    transformed_vertices[0].y / transformed_vertices[0].t,
                    transformed_vertices[0].z / transformed_vertices[0].t, 
                    1}));
                const Vec4 p_v1 = multiplyMatrixWithVec4(viewportTransformationMatrix, Vec4{
                    transformed_vertices[1].x / transformed_vertices[1].t,
                    transformed_vertices[1].y / transformed_vertices[1].t,
                    transformed_vertices[1].z / transformed_vertices[1].t, 
                    1});
                const Vec4 p_v2 = multiplyMatrixWithVec4(viewportTransformationMatrix,Vec4{
                    transformed_vertices[2].x / transformed_vertices[2].t,
                    transformed_vertices[2].y / transformed_vertices[2].t,
                    transformed_vertices[2].z / transformed_vertices[2].t, 
                    1});

                // RASTERIZE
                std::vector<Vec4> triangle_vertices = {p_v0, p_v1, p_v2};
                std::vector<Color> colors = {*c0, *c1, *c2};
                TriangleTransformations::rasterize(this->image,triangle_vertices,colors,camera->horRes,camera->verRes,this->depth);
            }
            else if(m->type == WIREFRAME_MESH)
            {
                // PERSPECTIVE DIVIDE
                Vec4 pers_v0 = Vec4({
                    transformed_vertices[0].x / transformed_vertices[0].t,
                    transformed_vertices[0].y / transformed_vertices[0].t,
                    transformed_vertices[0].z / transformed_vertices[0].t, 
                    1});
                Vec4 pers_v1 = Vec4{
                    transformed_vertices[1].x / transformed_vertices[1].t,
                    transformed_vertices[1].y / transformed_vertices[1].t,
                    transformed_vertices[1].z / transformed_vertices[1].t, 
                    1};
                Vec4 pers_v2 = Vec4{
                    transformed_vertices[2].x / transformed_vertices[2].t,
                    transformed_vertices[2].y / transformed_vertices[2].t,
                    transformed_vertices[2].z / transformed_vertices[2].t, 
                    1};
                Line l1 = {pers_v0, pers_v1, *c0, *c1};
                Line l2 = {pers_v1, pers_v2, *c1, *c2};
                Line l3 = {pers_v2, pers_v0, *c2, *c0};

                // CLIPPING

                bool l1_visible = Clipping::clipLine(l1);
                bool l2_visible = Clipping::clipLine(l2);
                bool l3_visible = Clipping::clipLine(l3);
                if(l1_visible)
                {
                    // VIEWPORT TRANSFORMATION
                    Vec4 v0 = multiplyMatrixWithVec4(viewportTransformationMatrix, l1.a);
                    Vec4 v1 = multiplyMatrixWithVec4(viewportTransformationMatrix, l1.b);
                    // RASTERIZE
                    Line l = {v0, v1, l1.aColor, l1.bColor};
                    Clipping::rasterize(this->image, l, this->depth,camera->horRes,camera->verRes);
                }
                if(l2_visible)
                {
                    Vec4 v0 = multiplyMatrixWithVec4(viewportTransformationMatrix, l2.a);
                    Vec4 v1 = multiplyMatrixWithVec4(viewportTransformationMatrix, l2.b);
                    Line l = {v0, v1, l2.aColor, l2.bColor};
                    Clipping::rasterize(this->image, l, this->depth,camera->horRes,camera->verRes);
                }
                if(l3_visible)
                {
                    Vec4 v0 = multiplyMatrixWithVec4(viewportTransformationMatrix, l3.a);
                    Vec4 v1 = multiplyMatrixWithVec4(viewportTransformationMatrix, l3.b);
                    Line l = {v0, v1, l3.aColor, l3.bColor};
                    Clipping::rasterize(this->image, l, this->depth,camera->horRes,camera->verRes);
                }


            }

        }

    }
}
