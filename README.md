# ShellExtension

Solution for this task should be a COM component that is ready for 64-bit. 
TIP: Use MS Visual Studio (it can be Express Edition).

Basic task:

Write COM component, that can be registered in MS Windows 7/8/10 64-bit. It should extend system shell to provide:
1.	1.	It should add menu item (example, "Calculate the Sum") in Windows Explorer’s context menu (right click on the list of files, folders should be ignored).
2.	2.	This menu item should write information about selected files into the log file.
3.	3.	Component should work on huge amount of selected files (1000+).
4.	4.	Try avoid using ATL components on your solution.
5.	
6.	Main task:
7.	
1.	1.	Each line in the log file should represents: short filename, size in human readable form, data of creation in human readable form (TIP: you may use BOOST::FILESYSTEM).
2.	2.	Component should only append lines into the log file.
3.	3.	Lines should be sorted by file names in alphabetical order.
4.	4.	In additional, it needs to calculate a per-byte sum (any other checksum is acceptable too) in 4 bytes variable (DWORD) for each file.
5.	5.	The calculation of checksum should be done asynchronously .
6.	6.	Using thread pool is a desirable solution (!!!).
7.	7.	Calculated information for each file should be logged as soon as possible, instead of at the end of processing of all files (sort order should be preserved).
8.	
9.	Additional requirements:
10.	
1.	1.	Source control systems (subversion, mercurial, git, etc.) are very useful for collaboration work. Use one of it to promote your solution (https://github.com, https://gitlab.com etc.).
2.	2.	C-style code is not welcome.
3.	3.	Use STL as much as possible.
4.	4.	Use C++11 or Boost for concurrency.
5.	5.	Implement your solution in cross-platform manner to be ready for use the business logic in Linux or Mac OS X application.
6.	6.	Cover your solution by unit tests (example, Google Test).
7.	7.	Test the business logic, not the COM specific code.
8.	8.	Get ready to describe all functions that you will use to implement the solution.
9.	
10.	Hint:
11.	
1.	1.	To solve the main task you have to implement IShellExtInit and IContextMenu.
2.	2.	Essential COM is a good book to learn COM technology.

Compile with VC 2017 + boost_1_65_1

Register component: regsvr32 ShellExtension.dll
Unregister component: regsvr32 /u ShellExtension.dll