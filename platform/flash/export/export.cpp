#include "export.h"
#include "platform/flash/logo.h"
#include "tools/editor/editor_import_export.h"
#include "tools/editor/editor_settings.h"
#include "scene/resources/texture.h"

class EditorExportPlatformFlash : public EditorExportPlatform {
    OBJ_TYPE(EditorExportPlatformFlash, EditorExportPlatform);
private:

public:
	virtual String get_name() const;
	virtual ImageCompression get_image_compression() const;
	virtual Ref<Texture> get_logo() const;

	virtual bool poll_devices() { return false; }
	virtual int get_device_count() const { return 0; }
	virtual String get_device_name(int p_device) const { return ""; }
	virtual String get_device_info(int p_device) const { return ""; }
	virtual Error run(int p_device,bool p_dumb=false) { return OK; }

	virtual bool can_export(String *r_error=NULL) const;

	virtual bool requires_password(bool p_debug) const { return false; }
	virtual String get_binary_extension() const;
	virtual Error export_project(const String& p_path,bool p_debug,bool p_dumb=false);
};

String EditorExportPlatformFlash::get_name() const {
    return "Adobe Flash";
}

EditorExportPlatform::ImageCompression EditorExportPlatformFlash::get_image_compression() const{
    return IMAGE_COMPRESSION_NONE;
}

Ref<Texture> EditorExportPlatformFlash::get_logo() const {
	Image img(_flash_logo);
	Ref<ImageTexture> logo = memnew(ImageTexture);
	logo->create_from_image(img);
    return logo;
}

bool EditorExportPlatformFlash::can_export(String *r_error) const {
    return false;
};

String EditorExportPlatformFlash::get_binary_extension() const {
    return "bin"; //TODO: Replace this with whatever flash's actual extension is
};

Error EditorExportPlatformFlash::export_project(const String& p_path,bool p_debug,bool p_dumb){
    return ERR_DOES_NOT_EXIST;
};

void register_flash_exporter(){
    Ref<EditorExportPlatformFlash> exporter = Ref<EditorExportPlatformFlash>(memnew(EditorExportPlatformFlash));
	EditorImportExport::get_singleton()->add_export_platform(exporter);
}
