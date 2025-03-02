#include "mesh_editor_plugin.h"

#include "tools/editor/editor_plugin.h"
#include "tools/editor/editor_node.h"
#include "scene/3d/mesh_instance.h"
#include "scene/3d/physics_body.h"
#include "scene/3d/body_shape.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/box_container.h"
#include "scene/3d/mesh_instance.h"
#include "scene/3d/navigation_mesh.h"
#include "spatial_editor_plugin.h"

void MeshInstanceEditor::_node_removed(Node *p_node) {

	if(p_node==node) {
		node=NULL;
		options->hide();
	}

}



void MeshInstanceEditor::edit(MeshInstance3D *p_mesh) {

	node=p_mesh;

}

void MeshInstanceEditor::_menu_option(int p_option) {

	Ref<Mesh> mesh = node->get_mesh();
	if (mesh.is_null()) {
		err_dialog->set_text("Mesh is empty!");
		err_dialog->popup_centered(Size2(100,50));
		return;
	}

	switch(p_option) {
		case MENU_OPTION_CREATE_STATIC_TRIMESH_BODY: {

			Ref<Shape3D> shape = mesh->create_trimesh_shape();
			if (shape.is_null())
				return;
			StaticBody3D *body = memnew( StaticBody3D );
			CollisionShape3D *cshape = memnew( CollisionShape3D );
			cshape->set_shape(shape);
			body->add_child(cshape);
			Node *owner = node==get_tree()->get_edited_scene_root() ? node : node->get_owner();

			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action("Create Static Trimesh");
			ur->add_do_method(node,"add_child",body);
			ur->add_do_method(body,"set_owner",owner);
			ur->add_do_method(cshape,"set_owner",owner);
			ur->add_do_reference(body);
			ur->add_undo_method(node,"remove_child",body);
			ur->commit_action();

		} break;
		case MENU_OPTION_CREATE_STATIC_CONVEX_BODY: {

			Ref<Shape3D> shape = mesh->create_convex_shape();
			if (shape.is_null())
				return;
			StaticBody3D *body = memnew( StaticBody3D );
			CollisionShape3D *cshape = memnew( CollisionShape3D );
			cshape->set_shape(shape);
			body->add_child(cshape);
			Node *owner = node==get_tree()->get_edited_scene_root() ? node : node->get_owner();

			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action("Create Static Trimesh");
			ur->add_do_method(node,"add_child",body);
			ur->add_do_method(body,"set_owner",owner);
			ur->add_do_method(cshape,"set_owner",owner);
			ur->add_do_reference(body);
			ur->add_undo_method(node,"remove_child",body);
			ur->commit_action();

		} break;
		case MENU_OPTION_CREATE_TRIMESH_COLLISION_SHAPE: {


			if (node==get_tree()->get_edited_scene_root()) {
				err_dialog->set_text("This doesn't work on scene root!");
				err_dialog->popup_centered(Size2(100,50));
				return;
			}
			Ref<Shape3D> shape = mesh->create_trimesh_shape();
			if (shape.is_null())
				return;
			CollisionShape3D *cshape = memnew( CollisionShape3D );
			cshape->set_shape(shape);

			Node *owner =  node->get_owner();

			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action("Create Static Trimesh");
			ur->add_do_method(node->get_parent(),"add_child",cshape);
			ur->add_do_method(node->get_parent(),"move_child",cshape,node->get_index()+1);
			ur->add_do_method(cshape,"set_owner",owner);
			ur->add_do_reference(cshape);
			ur->add_undo_method(node->get_parent(),"remove_child",cshape);
			ur->commit_action();

		} break;
		case MENU_OPTION_CREATE_CONVEX_COLLISION_SHAPE: {


			if (node==get_tree()->get_edited_scene_root()) {
				err_dialog->set_text("This doesn't work on scene root!");
				err_dialog->popup_centered(Size2(100,50));
				return;
			}
			Ref<Shape3D> shape = mesh->create_convex_shape();
			if (shape.is_null())
				return;
			CollisionShape3D *cshape = memnew( CollisionShape3D );
			cshape->set_shape(shape);

			Node *owner =  node->get_owner();

			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action("Create Static Trimesh");
			ur->add_do_method(node->get_parent(),"add_child",cshape);
			ur->add_do_method(node->get_parent(),"move_child",cshape,node->get_index()+1);
			ur->add_do_method(cshape,"set_owner",owner);
			ur->add_do_reference(cshape);
			ur->add_undo_method(node->get_parent(),"remove_child",cshape);
			ur->commit_action();

		} break;
		case MENU_OPTION_CREATE_NAVMESH: {




			Ref<NavigationMesh> nmesh = memnew( NavigationMesh );

			if (nmesh.is_null())
				return;

			nmesh->create_from_mesh(mesh);
			NavigationMeshInstance *nmi = memnew(  NavigationMeshInstance );
			nmi->set_navigation_mesh(nmesh);

			Node *owner = node==get_tree()->get_edited_scene_root() ? node : node->get_owner();

			UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();
			ur->create_action("Create Navigation Mesh");

			ur->add_do_method(node,"add_child",nmi);
			ur->add_do_method(nmi,"set_owner",owner);

			ur->add_do_reference(nmi);
			ur->add_undo_method(node,"remove_child",nmi);
			ur->commit_action();
		} break;
		case MENU_OPTION_CREATE_OUTLINE_MESH: {

			outline_dialog->popup_centered(Size2(200,80));
		} break;
	}

}

void MeshInstanceEditor::_create_outline_mesh() {

	Ref<Mesh> mesh = node->get_mesh();
	if (mesh.is_null()) {
		err_dialog->set_text("MeshInstance3D lacks a Mesh!");
		err_dialog->popup_centered(Size2(100,50));
		return;
	}

	Ref<Mesh> mesho = mesh->create_outline(outline_size->get_val());

	if (mesho.is_null()) {
		err_dialog->set_text("Could not create outline!");
		err_dialog->popup_centered(Size2(100,50));
		return;
	}

	MeshInstance3D *mi = memnew( MeshInstance3D );
	mi->set_mesh(mesho);
	Node *owner=node->get_owner();
	if (get_tree()->get_edited_scene_root()==node) {
		owner=node;
	}

	UndoRedo *ur = EditorNode::get_singleton()->get_undo_redo();

	ur->create_action("Create Outline");

	ur->add_do_method(node,"add_child",mi);
	ur->add_do_method(mi,"set_owner",owner);

	ur->add_do_reference(mi);
	ur->add_undo_method(node,"remove_child",mi);
	ur->commit_action();
}

void MeshInstanceEditor::_bind_methods() {

	ObjectTypeDB::bind_method("_menu_option",&MeshInstanceEditor::_menu_option);
	ObjectTypeDB::bind_method("_create_outline_mesh",&MeshInstanceEditor::_create_outline_mesh);
}

MeshInstanceEditor::MeshInstanceEditor() {


	options = memnew( MenuButton );
	//add_child(options);
	SpatialEditor::get_singleton()->add_control_to_menu_panel(options);

	options->set_text("Mesh");
	options->get_popup()->add_item("Create Trimesh Static Body",MENU_OPTION_CREATE_STATIC_TRIMESH_BODY);
	options->get_popup()->add_item("Create Convex Static Body",MENU_OPTION_CREATE_STATIC_CONVEX_BODY);
	options->get_popup()->add_separator();
	options->get_popup()->add_item("Create Trimesh Collision Sibling",MENU_OPTION_CREATE_TRIMESH_COLLISION_SHAPE);
	options->get_popup()->add_item("Create Convex Collision Sibling",MENU_OPTION_CREATE_CONVEX_COLLISION_SHAPE);
	options->get_popup()->add_separator();
	options->get_popup()->add_item("Create Navigation Mesh",MENU_OPTION_CREATE_NAVMESH);
	options->get_popup()->add_separator();
	options->get_popup()->add_item("Create Outline Mesh..",MENU_OPTION_CREATE_OUTLINE_MESH);

	options->get_popup()->connect("item_pressed", this,"_menu_option");

	outline_dialog = memnew( ConfirmationDialog );
	outline_dialog->set_title("Outline Size: ");
	outline_size = memnew( SpinBox );
	outline_size->set_min(0.001);
	outline_size->set_max(1024);
	outline_size->set_step(0.001);
	outline_size->set_val(0.05);
	outline_dialog->add_child(outline_size);
	outline_dialog->set_child_rect(outline_size);
	add_child(outline_dialog);
	outline_dialog->connect("confirmed",this,"_create_outline_mesh");

}


void MeshInstanceEditorPlugin::edit(Object *p_object) {

	mesh_editor->edit(p_object->cast_to<MeshInstance3D>());
}

bool MeshInstanceEditorPlugin::handles(Object *p_object) const {

	return p_object->is_type("MeshInstance3D");
}

void MeshInstanceEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		mesh_editor->options->show();
	} else {

		mesh_editor->options->hide();
		mesh_editor->edit(NULL);
	}

}

MeshInstanceEditorPlugin::MeshInstanceEditorPlugin(EditorNode *p_node) {

	editor=p_node;
	mesh_editor = memnew( MeshInstanceEditor );
	editor->get_viewport()->add_child(mesh_editor);

	mesh_editor->options->hide();
}


MeshInstanceEditorPlugin::~MeshInstanceEditorPlugin()
{
}


