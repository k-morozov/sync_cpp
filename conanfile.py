from conans import ConanFile, CMake

class YourProjectConan(ConanFile):
    name = "your_project"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"asan": [True, False], "ubsan": [True, False]}
    default_options = {"asan": False, "ubsan": False}
    generators = "cmake"

    # Указываем конкретную версию Google Test
    requires = "gtest/1.11.0"

    def configure(self):
        if self.settings.compiler == "clang":
            if self.settings.build_type == "Debug":
                self.options["gtest"].asan = True  # Включаем ASan для Debug-сборки
                self.options["gtest"].ubsan = True  # Включаем UBSan для Release-сборки

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
