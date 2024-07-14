import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class DemkiConan(ConanFile):
	settings = "os", "compiler", "build_type", "arch"
	requires = ["qt/6.7.1",
				"glfw/3.3.8",
				"tinyobjloader/2.0.0-rc10",
				"vulkan-loader/1.3.268.0",
				"vulkan-headers/1.3.268.0",
				"glad/0.1.36",
				"glm/0.9.9.8",
				"stb/cci.20220909",
				"pfr/2.1.0",
				"tclap/1.2.5"]
	generators = "CMakeDeps"

	def requirements(self):
		self.requires("libxml2/2.11.4", override=True)

	def configure(self):
		self.options["glad"].gl_profile = 'core'
		self.options["glad"].gl_version = '4.6'

		os.environ["NOT_ON_C3I"] = "1"
		#Qt6 configuration
		# Enabled
		self.options["qt"]["gui"] = True
		self.options["qt"]["shared"] = True
		self.options["qt"]["widgets"] = True
		self.options["qt"]["with_pcre2"] = True
		self.options["qt"]["with_libpng"] = True
		self.options["qt"]["with_freetype"] = True
		self.options["qt"]["with_harfbuzz"] = True
		self.options["qt"]["with_doubleconversion"] = True

		self.options["qt"]["with_vulkan"] = True

		if self.settings.os == "Windows":
			self.options["qt"]["opengl"] = "dynamic"
		elif self.settings.os == "Linux":
			self.options["qt"]["opengl"] = "desktop"
			self.options["qt"]["qtdoc"] = False
			self.options["qt"]["qttools"] = False
			self.options["qt"]["qtdeclarative"] = False
			self.options["qt"]["qttranslations"] = False

		# Disabled
		#self.options["qt"]["opengl"] = "no"
		self.options["qt"]["qt3d"] = False
		self.options["qt"]["qtsvg"] = False
		self.options["qt"]["qtcoap"] = False
		self.options["qt"]["qtgrpc"] = False
		self.options["qt"]["qtmqtt"] = False
		self.options["qt"]["openssl"] = False
		self.options["qt"]["with_pq"] = False
		self.options["qt"]["qtopcua"] = False
		self.options["qt"]["qtscxml"] = False
		self.options["qt"]["qtcharts"] = False
		self.options["qt"]["qtgraphs"] = False
		self.options["qt"]["qtlottie"] = False
		self.options["qt"]["qtspeech"] = False
		self.options["qt"]["qt5compat"] = False
		self.options["qt"]["qtquick3d"] = False
		self.options["qt"]["qtsensors"] = False
		self.options["qt"]["qtwayland"] = False
		self.options["qt"]["qtwebview"] = False
		self.options["qt"]["with_dbus"] = False
		self.options["qt"]["with_glib"] = False
		self.options["qt"]["with_md4c"] = False
		self.options["qt"]["with_odbc"] = False
		self.options["qt"]["with_zstd"] = False
		self.options["qt"]["qtactiveqt"] = False
		self.options["qt"]["qtlocation"] = False
		self.options["qt"]["with_mysql"] = False
		self.options["qt"]["with_brotli"] = False
		self.options["qt"]["qtdatavis3d"] = False
		self.options["qt"]["qtserialbus"] = False
		self.options["qt"]["qtwebengine"] = False
		self.options["qt"]["qthttpserver"] = False
		self.options["qt"]["qtmultimedia"] = False
		self.options["qt"]["qtserialport"] = False
		self.options["qt"]["qtwebchannel"] = False
		self.options["qt"]["qtwebsockets"] = False
		self.options["qt"]["with_libjpeg"] = False
		self.options["qt"]["with_sqlite3"] = False
		self.options["qt"]["qtnetworkauth"] = False
		self.options["qt"]["qtpositioning"] = False
		self.options["qt"]["qtshadertools"] = False
		self.options["qt"]["qtconnectivity"] = False
		self.options["qt"]["qtimageformats"] = False
		self.options["qt"]["qtquicktimeline"] = False
		self.options["qt"]["qtremoteobjects"] = False
		self.options["qt"]["qtlanguageserver"] = False
		self.options["qt"]["qtquick3dphysics"] = False
		self.options["qt"]["disabled_features"] = None
		self.options["qt"]["qtvirtualkeyboard"] = False
		self.options["qt"]["multiconfiguration"] = False
		self.options["qt"]["qtquickeffectmaker"] = False

	def generate(self):
		tc = CMakeToolchain(self)
		tc.presets_prefix = "conan-" + self.settings.get_safe("compiler")
		tc.generate()

	def layout(self):
		# We make the assumption that if the compiler is msvc the
		# CMake generator is multi-config
		multi = True if self.settings.get_safe("compiler") == "msvc" else False
		if multi:
			self.folders.generators = os.path.join("generators")
		else:
			self.folders.generators = os.path.join("generators", str(self.settings.build_type))
