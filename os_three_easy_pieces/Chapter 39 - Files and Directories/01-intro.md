### Key Notes on Files and Directories

#### Operating System Abstractions

- **Process**: Virtualization of the CPU, allowing a program to run as if it has its own processor(s).
- **Address Space**: Virtualization of memory, giving the illusion of private, isolated memory.
- **Persistent Storage**: A critical piece of virtualization, enabling data to be stored permanently (e.g., hard drives, SSDs).

#### Persistent Storage Management

- **Key Questions**:
    - How should the OS manage persistent devices?
    - What APIs should be provided?
    - What implementation aspects are critical?
- Focus on improving **performance** and **reliability** of persistent data management.

#### Files

- **Definition**: A file is a linear array of bytes that can be read or written.
- **Low-Level Name**: Each file has a low-level identifier, often referred to as an **inode number** (i-number).
- **OS Role**: The OS does not interpret the file's content (e.g., text, image, code) but ensures data is stored persistently and retrieved accurately.

#### Directories

- **Definition**: A directory is a structure containing a list of (user-readable name, low-level name) pairs.
- **Low-Level Name**: Like files, directories also have an inode number.
- **Contents**: Directories map user-readable names to low-level names and can contain files or other directories.
- **Hierarchy**: Directories can be nested to form a **directory tree** or **hierarchy**.
    - Root directory: `/` (in UNIX systems).
    - Example: `/foo/bar.txt` refers to a file `bar.txt` in the directory `foo`, which is under the root `/`.

#### Naming in File Systems

- **Importance of Naming**:
    - Names are critical for accessing resources.
    - UNIX systems use a unified naming system for files, devices, pipes, processes, etc.
- **File Names**:
    - Can have two parts: `name.extension` (e.g., `bar.txt`).
    - The extension (e.g., `.txt`, `.jpg`, `.mp3`) is a convention to indicate file type but is not enforced by the OS.
- **Uniqueness**:
    - Files and directories can have the same name if they are in different locations in the directory tree.
    - Example: `/foo/bar.txt` and `/bar/foo/bar.txt` are distinct files.

#### Key Takeaways

- The file system provides a **convenient naming mechanism** for all resources, making the system simpler and more modular.
- Persistent storage is crucial for retaining data across power losses, and the OS must manage it carefully.
- The directory tree structure allows for organized and hierarchical storage of files and directories.

#### Example Directory Tree

- **Valid Directories**: `/`, `/foo`, `/bar`, `/bar/bar`, `/bar/foo`.
- **Valid Files**: `/foo/bar.txt`, `/bar/foo/bar.txt`.

#### Tip: Think Carefully About Naming

- Naming is a fundamental aspect of system design.
- A unified naming system simplifies the conceptual model and enhances modularity.