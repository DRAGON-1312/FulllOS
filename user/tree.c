#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
printIndent(int depth)
{
  for(int i = 0; i < depth; i++){
    printf("  ");   // two spaces per level
  }
}

void 
tree(char *path, char *name, int depth)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;
  
  // Open the current path
  if((fd = open(path, 0)) < 0){
    printf("tree: cannot open %s\n", path);
    return;
  }
  
  // Get information about the path
  // fstat(fd, &st) lấy thông tin metadata của file/directory đang được mở bởi fd, rồi lưu vào struct stat st
  if(fstat(fd, &st) < 0){
    printf("tree: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  // If this path is a file, just print it
  if(st.type != T_DIR){
    printIndent(depth);
    printf("%s\n", name);
    close(fd);
    return;
  }
  
  // If this path is a directory, print it with /
  printIndent(depth);
  printf("%s/\n", name);
  
  // Check path length before building child paths
  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
    printf("tree: path too long\n");
    close(fd);
    return;
  }
  
  // Prepare buffer: buf = path + "/"
  strcpy(buf, path); // copy đường dẫn hiện tại vào buf
  p = buf + strlen(buf); // cho p trỏ tới cuối chuỗi
  *p++ = '/'; // '/' thêm dấu "/" vào cuối path, rồi dời p tới vị trí dùng để nối tên file/thư mục con (p trỏ sang vị trí kế tiếp để lát nữa copy tên con vào
  
  // Read each directory entry
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0) // Nếu entry này không được sử dụng / entry rỗng
      continue;

    // Copy directory entry name safely
    char childname[DIRSIZ + 1]; // Ô dư ra dùng để chứa \0
    memmove(childname, de.name, DIRSIZ); // Copy đúng DIRSIZ ký tự từ de.name sang childname 
    childname[DIRSIZ] = 0; // Gắn ký tự kết thúc chuỗi vào cuối. 0 ở đây chính là '\0'

    // Skip "." and ".." to avoid infinite recursion (tên entry là ".":  đại diện cho chính thư mục hiện tại, "..": đại diện cho thư mục cha thi skip) 
    if(strcmp(childname, ".") == 0 || strcmp(childname, "..") == 0)
      continue;

    // Build full child path: parent/child
    memmove(p, childname, DIRSIZ); // copy tên file/thư mục con vào sau "parent/" trong buf
    p[DIRSIZ] = 0; // thêm ký tự kết thúc chuỗi '\0'


    // Lấy metadata của child hiện tại
    // buf lúc này là full path của child, ví dụ "a/b" hoặc "a/aa"
    struct stat st2;
    if(stat(buf, &st2) < 0){ // Nếu không lấy được metadata thì báo lỗi và bỏ qua child này
      printf("tree: cannot stat %s\n", buf); 
      continue; // continue để đọc entry tiếp theo
    }

    // Nếu child là directory
    if(st2.type == T_DIR){
      tree(buf, childname, depth + 1); // gọi đệ quy tree(...) để in thư mục đó và duyệt tiếp bên trong nó
    } else { // Nếu child là file
      printIndent(depth + 1);
      printf("%s\n", childname); // chỉ in tên file ra, không cần duyệt tiếp
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc == 1){  // nếu user không truyền argument
    tree(".", ".", 0); // chương trình sẽ in cây thư mục của current directory (Dấu "." nghĩa là thư mục hiện tại)
  } else {
    tree(argv[1], argv[1], 0);
  }

  exit(0);
}
