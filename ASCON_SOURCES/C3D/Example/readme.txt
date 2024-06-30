// \English
In the Example\Demo directory there is test.exe test application ready for use for a Windows,
demonstrating the possibilities of the geometric kernel c3d.dll
After starting the application test.exe must enter the key and signature by:
Menu->Help->License_Key, Signature.

In the Example\Models directory there are examples of models in the form of files *.c3d, *.stp, *.x_t, *.stl,
can operate with a test application test.exe.

In the Example\Source directory there are the source code of test.exe test application,
and CMakeLists.txt file to generate the test application project/solution.

To compile the test application of the geometric kernel C3D, follow these steps.

1. Create the a new folder (for example "TestApp") in a comfortable working position.

2. Choose the archive corresponding to the development environment in the <C3D> catalog.

3. Copy <Include>, <Debug>, and <Release> folders from the chosen archive to a new folder.

4. Copy <Source> folder from the <Example> archive to a new folder.

5. Make sure that the new folder contains four directories: <Debug>, <Include>, <Release>, and <Source>.

6. Install CMake, the installation process by selecting the option "Add CMake to the system PATH for all users".

7. Create a solution for test application using the following steps.
   Run CMake.
   To "Where is the source code", specify the directory <path_to_testapp>\TestApp\Source.
   For "Where to build the binaries", specify the directory <path_to_testapp>\TestApp\Build.
   Click the "Configure" button to configure the project.
   At the request "Create Directory" confirm agreement to create the directory <path_to_testapp>\TestApp\Build.
   On request "Specify the generator for this project" select the appropriate version of C3D configuration development environment.
   Click the "Generate" to generate the project/solution files.

8. Run your solution TestApp\Build\Test.sln for the test application in the development environment.

9. If there are some troubles with installation of CMake, use project files in the Example.
   That project files were generated with Unicode!
   Copy files of your development environment: Test.vcxproj, Test.vcxproj.filters to the folder created by step 1.
   Run the project Test.vcxproj in the development environment.

10.To activate the geometrical kernel C3D before compiling change the call
   EnableMathModules(...) in the constructor of a "Manager" in the file test_manager.cpp by the real key and signature.

11.After compilation, run the generated test application test.exe from the catalog
   Debug or Release, respectively.


// \Russian
В каталоге Example\Demo находится готовое для использования в системе Windows тестовое приложение test.exe, 
демонстрирующее возможности геометрического ядра c3d.dll
После запуска приложения test.exe необходимо ввести ключ и сигнатуру выбрав: 
Меню->Помощь->Лицензионный_ключ, Сигнатура.

В каталоге Example\Models находятся примеры моделей в виде файлов *.c3d, *.stp, *.x_t, *.stl, 
с которыми может работать тестовое приложение test.exe.

В каталоге Example\Source находятся исходные тексты тестового приложения test.exe, 
а также файл CMakeLists.txt для генерации проекта/решения тестового приложения.

Для компиляции тестового приложения геометрического ядра C3D выполните следующие действия.

1. Создайте в удобном для работы месте новый каталог, например, "TestApp".

2. В каталоге <C3D> выберите архив, соответствующий среде разработки.

3. Скопируйте из выбранного архива каталоги <Include>, <Debug> и <Release> в новый каталог. 

4. Скопируйте из архива <Example> каталог <Source> в новый каталог.

5. Убедитесь, что в новом каталоге находятся четыре папки: <Debug>, <Include>, <Release> и <Source>.

6. Установите CMake, выбрав в процессе установки опцию "Add CMake to the system PATH for all users".

7. Создайте проект/решение тестового приложения, выполнив следующие действия. 
   Запустите CMake.
   Для "Where is the source code" укажите каталог <path_to_testapp>\TestApp\Source.
   Для "Where to build the binaries" укажите каталог <path_to_testapp>\TestApp\Build.
   Нажмите кнопку "Configure" для конфигурирования проекта/решения.
   На запрос "Create Directory" подтвердите согласие на создание каталога <path_to_testapp>\TestApp\Build.
   По запросу "Specify the generator for this project" укажите соответствующую версии C3D конфигурацию среды разработки.
   Нажмите кнопку "Generate" для генерации файлов проекта/решения. 

8. Запустите созданное решение TestApp\Build\Test.sln тестового приложения в Вашей среде разработки.

9. Если при установке CMake возникли проблемы, 
   то можете воспользоваться готовым проектом Test.vcxproj, расположенными в каталоге Example.
   Проект подготовлен для конфигурации Unicode!
   Для сборки проекта скопируйте файлы проекта: Test.vcxproj, Test.vcxproj.filters в каталог, созданный на шаге 1.
   Запустите проект Test.vcxproj тестового приложения в Вашей среде разработки.

10.Для активации геометрического ядра C3D перед компиляцией измените вызов метода 
   EnableMathModules(...) в конструкторе объекта "Manager" в файле test_manager.cpp с реальными ключом и сигнатурой.

11.После компиляции запустите созданное тестовое приложение test.exe из каталога 
   Debug или Release, соответственно.
