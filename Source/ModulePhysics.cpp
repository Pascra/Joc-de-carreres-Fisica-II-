#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include <iostream>

#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	TraceLog(LOG_INFO, "Creating Physics 2D environment");

	// Crear el mundo f�sico con la gravedad especificada
	world = new b2World(b2Vec2(GRAVITY_X, GRAVITY_Y));
	if (!world)
	{
		TraceLog(LOG_ERROR, "Could not create physics world");
		return false;
	}

	// Configurar este m�dulo como listener para colisiones
	world->SetContactListener(this);
	TraceLog(LOG_INFO, "Physics world created successfully with contact listener");

	// Crear un cuerpo base para joints u otros usos
	b2BodyDef bd;
	ground = world->CreateBody(&bd);
	if (!ground)
	{
		TraceLog(LOG_ERROR, "Could not create ground body");
		return false;
	}

	TraceLog(LOG_INFO, "Ground body created successfully");

	return true;
}



update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 8, 3);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureB()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	// Debug draw for sensors
	if (debug)
	{
		for (b2Body* body = world->GetBodyList(); body; body = body->GetNext())
		{
			for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
			{
				if (fixture->IsSensor())
				{
					b2Shape* shape = fixture->GetShape();
					if (shape->GetType() == b2Shape::e_polygon)
					{
						b2PolygonShape* polygonShape = (b2PolygonShape*)shape;
						int32 count = polygonShape->m_count;

						b2Vec2 prevVertex = body->GetWorldPoint(polygonShape->m_vertices[count - 1]);
						for (int32 i = 0; i < count; ++i)
						{
							b2Vec2 currentVertex = body->GetWorldPoint(polygonShape->m_vertices[i]);
							DrawLine(METERS_TO_PIXELS(prevVertex.x), METERS_TO_PIXELS(prevVertex.y), METERS_TO_PIXELS(currentVertex.x), METERS_TO_PIXELS(currentVertex.y), GREEN);
							prevVertex = currentVertex;
						}
					}
				}
			}
		}
	}


	return UPDATE_CONTINUE;
}


PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	LOG("Creating rectangle sensor at (%d, %d) with size (%d, %d)", x, y, width, height);

	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	if (!b)
	{
		LOG("Failed to create sensor body.");
		delete pbody;
		return nullptr;
	}

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = width;
	pbody->height = height;

	LOG("Sensor created successfully at (%d, %d)", x, y);
	return pbody;
}


PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	b2ChainShape shape;
	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	if (!b->CreateFixture(&fixture))
	{
		delete[] p;    // Liberar memoria en caso de error
		delete pbody;  // Liberar el PhysBody si falla la creaci�n del fixture
		world->DestroyBody(b); // Eliminar el cuerpo creado
		return nullptr;
	}

	delete[] p; // Liberar memoria din�mica correctamente

	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}


// 
update_status ModulePhysics::PostUpdate()
{
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	// Si debug es false, retornamos inmediatamente
	if (!debug)
		return UPDATE_CONTINUE;

	b2Body* mouseSelect = nullptr;
	Vector2 mousePosition = GetMousePosition();
	b2Vec2 pMousePosition = b2Vec2(PIXEL_TO_METERS(mousePosition.x), PIXEL_TO_METERS(mousePosition.y));

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();

				DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->m_count;
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
			}
			break;
			}

			// TODO 1: If mouse button 1 is pressed ...
			// test if the current body contains mouse position
			if (mouse_joint == nullptr && mouseSelect == nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

				if (f->TestPoint(pMousePosition)) {
					mouseSelect = b;
				}
			}
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property
	if (mouseSelect) {
		b2MouseJointDef def;

		def.bodyA = ground;
		def.bodyB = mouseSelect;
		def.target = pMousePosition;
		def.damping = 0.5f;
		def.stiffness = 20.f;
		def.maxForce = 100.f * mouseSelect->GetMass();

		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	}

	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points
	else if (mouse_joint && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		mouse_joint->SetTarget(pMousePosition);
		b2Vec2 anchorPosition = mouse_joint->GetBodyB()->GetPosition();
		anchorPosition.x = METERS_TO_PIXELS(anchorPosition.x);
		anchorPosition.y = METERS_TO_PIXELS(anchorPosition.y);

		DrawLine(anchorPosition.x, anchorPosition.y, mousePosition.x, mousePosition.y, RED);
	}

	// TODO 4: If the player releases the mouse button, destroy the joint
	else if (mouse_joint && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	if (world)
	{
		// Iterar sobre todos los cuerpos en el mundo
		for (b2Body* body = world->GetBodyList(); body != nullptr; )
		{
			b2Body* nextBody = body->GetNext();

			// Liberar el PhysBody asociado
			PhysBody* physBody = (PhysBody*)body->GetUserData().pointer;
			if (physBody)
			{
				//delete physBody; // Liberar memoria del PhysBody
				body->GetUserData().pointer = 0; // Limpiar el puntero
			}

			world->DestroyBody(body); // Destruir el cuerpo f�sico
			body = nextBody;
		}

		// Destruir el joint del rat�n si existe
		if (mouse_joint)
		{
			world->DestroyJoint(mouse_joint);
			mouse_joint = nullptr;
		}

		delete world; // Liberar el mundo f�sico
		world = nullptr;
	}

	LOG("Physics world destroyed successfully");
	return true;
}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	LOG("BeginContact called"); // Mensaje para confirmar que este m�todo se ejecuta

	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	b2BodyUserData userDataA = fixtureA->GetBody()->GetUserData();
	b2BodyUserData userDataB = fixtureB->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)userDataA.pointer;
	PhysBody* physB = (PhysBody*)userDataB.pointer;

	if (physA && physA->listener)
	{
		LOG("Calling listener for Fixture A");
		physA->listener->OnCollision(physA, physB);
	}

	if (physB && physB->listener)
	{
		LOG("Calling listener for Fixture B");
		physB->listener->OnCollision(physB, physA);
	}
}




