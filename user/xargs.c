#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h" // dùng để khai báo mảng argument (MAXARG: 32)

#define MAXLINE 512

int
main(int argc, char *argv[])
// argc: số lượng argument được truyền vào chương trình từ command line.
// argv: mảng các chuỗi, chứa từng argument user truyền vào.
{
  char *new_argv[MAXARG]; // Đây là mảng argument mới sẽ truyền vào exec
  char buf[MAXLINE]; // buf dùng để chứa một dòng đọc từ stdin.
  char ch; // ch dùng để đọc từng ký tự
  int i;
  
  if(argc < 2){ // Không có command nào để xargs chạy, nên phải báo lỗi. (argv[0] = "xargs": program name)
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }
  
  if(argc >= MAXARG){ // chừa chỗ cho argument mới từ stdin và số 0 kết thúc mảng.
    fprintf(2, "xargs: too many arguments\n");
    exit(1);
  }
  
  for(i = 1; i < argc; i++){
    // Mình bỏ qua argv[0] vì đó là tên chương trình xargs
    new_argv[i - 1] = argv[i];
  }
  
  while(1){
    int index = 0;
    int n;

    // n là số byte đọc được. Đọc 1 ký tự thì n thường là 1. Nếu n = 0 là hết input.
    while((n = read(0, &ch, 1)) > 0){ // read(0, &ch, 1): nghĩa là đọc 1 ký tự từ stdin. File descriptor 0 là standard input.
      if(ch == '\n'){
        break;
      }

      if(index >= MAXLINE - 1){
        fprintf(2, "xargs: input line too long\n");
        exit(1);
      }

      buf[index++] = ch;
    }

    if(n <= 0 && index == 0){ // nếu không đọc được gì nữa và dòng hiện tại cũng rỗng, thì kết thúc chương trình.
      break;
    }

    buf[index] = '\0'; // đánh dấu kết thúc chuỗi C.
    
    new_argv[argc - 1] = buf; // thêm dòng stdin vào cuối argument của command.
    new_argv[argc] = 0; // Mảng argument truyền cho exec bắt buộc phải kết thúc bằng con trỏ null. (ket thuc = 0)
    
    if(fork() == 0){ // fork(): tạo process con.
    // child process
      exec(new_argv[0], new_argv); // chạy command thật sự, ví dụ chạy echo
      // Nếu exec thành công thì process hiện tại bị thay bằng chương trình mới nên các dòng sau exec không chạy.

      fprintf(2, "xargs: exec %s failed\n", new_argv[0]); // chỉ chạy khi exec thất bại.
      exit(1);
    } else {
    // parent process
      wait(0); // đợi process con chạy xong rồi mới đọc dòng tiếp theo.
    }
  }

  exit(0);
}
