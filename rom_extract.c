#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CRITICAL_LEVEL 10

void init(void);
void usage(void);
void process(void);
void cleanup(void);
void error(int, char *, ...);

/*
Packet types for run-length encoding.
*/
enum {
  RLE_REPEAT = 0xb0,
  RLE_REPEAT_END = 0xb1,
  RLE_COPY = 0xb2,
  RLE_COPY_END = 0xb3
};

/*
0 for size means the last entry.
-1 for position means a discarded section.
*/
typedef struct MemoryMapEntry {
  long int size;
  long int position;
} MemoryMapEntry;

MemoryMapEntry default_memory_map[] = {{0x9000, 0x0}, {0x3e20, 0xc1e0}, {0}};

MemoryMapEntry defender_memory_map[] = {
    {0x3800, 0x8000}, {0x3000, 0xD000}, {0x4800, -1}, {0xE20, 0xC1E0}, {0}};

MemoryMapEntry *memory_map = default_memory_map;

char *app_name = NULL;
char *input_name = NULL;
char *output_name = NULL;
int errlevel = 0;
int verbose = 0;
long int input_size, input_pos;
long int output_size, output_pos;
unsigned char *input_data = NULL;
unsigned char *output_data = NULL;
FILE *input_file = NULL;
FILE *output_file = NULL;
int memory_map_pos;

int main(int argc, char *argv[]) {
  char *arg;

  app_name = argv[0];

  while (argv++, --argc) {
    if (**argv == '-') {
      for (arg = *argv + 1; *arg; arg++) {
        if (strchr("io", *arg) && ((--argc == 0) || **(++argv) == '-')) {
          error(5, "the %c option requires an argument\n", *arg);
          usage();
        }

        switch (*arg) {
        case '?':
        case 'h':
          usage();

        case 'v':
          verbose = 1;
          break;

        case 'i':
          if (input_name != NULL) {
            error(5, "too many input files specified\n");
            usage();
          }
          input_name = *argv;
          break;

        case 'o':
          if (output_name != NULL) {
            error(5, "too many output files specified\n");
            usage();
          }
          output_name = *argv;
          break;

        case 'd':
          memory_map = defender_memory_map;
          break;

        default:
          error(5, "unknown option '%c'\n", *arg);
          usage();
        }
      }
    } else {
      if (input_name == NULL) {
        input_name = *argv;
      } else if (output_name == NULL) {
        output_name = *argv;
      } else {
        error(5, "too many parameters\n");
        usage();
      }
    }
  }

  init();
  process();
  cleanup();

  error(0, "Done!\n");
  return errlevel;
}

void error(int level, char *format, ...) {
  va_list arg;

  fprintf(stderr, "# %s: ", app_name ? app_name : "unknown");

  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);

  if (level > errlevel) {
    errlevel = level;
  }

  if (level >= CRITICAL_LEVEL) {
    cleanup();
    exit(1);
  }
}

unsigned char current_byte(void) {
  if (input_pos < 1) {
    error(10, "unexpected start of file\n");
  }

  return input_data[input_pos - 1];
}

unsigned char read_byte(void) {
  if (input_pos < 1) {
    error(10, "unexpected start of file\n");
  }

  return input_data[--input_pos];
}

unsigned int read_word(void) {
  if (input_pos < 2) {
    error(10, "unexpected start of file\n");
  }

  input_pos -= 2;

  return ((unsigned)(input_data[input_pos + 1] << 8) + input_data[input_pos]);
}

void write_byte(unsigned char b) {
  if (memory_map_pos < 0) {
    if (verbose && output_pos-- == 0) {
      printf("EXE...");
    }

    return;
  }

  if (verbose && memory_map[memory_map_pos].position >= 0 &&
      output_pos == memory_map[memory_map_pos].position +
                        memory_map[memory_map_pos].size) {
    printf("%04lX... ", output_pos - 1);
  }

  output_pos--;

  if (memory_map[memory_map_pos].position >= 0) {
    output_data[output_pos] = b;
  }

  if (output_pos == memory_map[memory_map_pos].position) {
    if (verbose && memory_map[memory_map_pos].position >= 0) {
      printf("...%04lX ", output_pos);
    }

    memory_map_pos--;

    if (memory_map_pos < 0) {
      output_pos = 0;
    } else {
      output_pos =
          memory_map[memory_map_pos].position + memory_map[memory_map_pos].size;
    }
  }
}

void usage(void) {
  fprintf(stderr, "Usage: %s [-i] inputfile [[-o] outputfile] [-v] [-d]\n",
          app_name);

  cleanup();
  exit(errlevel);
}

void cleanup(void) {
  if (input_file != NULL) {
    fclose(input_file);
  }

  if (output_file != NULL) {
    fclose(output_file);
  }

  if (input_data != NULL) {
    free(input_data);
  }

  if (output_data != NULL) {
    free(output_data);
  }
}

void init(void) {
  long int i;

  if (input_name) {
    input_file = fopen(input_name, "rb");
  }

  if (input_file == NULL) {
    error(10, "couldn't open input file\n");
  }

  fseek(input_file, 0, SEEK_END);
  input_size = ftell(input_file);
  fseek(input_file, 0, SEEK_SET);

  input_data = malloc(input_size);

  if (input_data == NULL) {
    error(10, "couldn't allocate buffer for input file\n");
  }

  if (fread(input_data, input_size, 1, input_file) != 1) {
    error(10, "couldn't read input file\n");
  }

  input_pos = input_size;

  if (!output_name) {
    output_name = "williams.rom";
  }

  output_file = fopen(output_name, "wb");

  if (output_file == NULL) {
    error(10, "couldn't open output file\n");
  }

  memory_map_pos = 0;
  output_size = 0;

  while (memory_map[memory_map_pos].size > 0) {
    output_pos =
        memory_map[memory_map_pos].position + memory_map[memory_map_pos].size;

    if (memory_map[memory_map_pos].position >= 0 && output_pos > output_size) {
      output_size = output_pos;
    }

    memory_map_pos++;
  }

  memory_map_pos--;

  output_data = malloc(output_size);

  if (output_data == NULL) {
    error(10, "couldn't allocate buffer for output file\n");
  }

  for (i = 0; i < output_size; i++) {
    output_data[i] = 0xff;
  }
}

void process(void) {
  unsigned char byte;
  unsigned int count;
  int finished = 0;

  while (1) {
    while (current_byte() != 0x00) {
      read_byte();
    }

    read_byte();

    if (current_byte() == 0x40) {
      read_byte();
      break;
    }
  }

  if (verbose) {
    printf("%05lx-%05lx: ..            details\n", input_pos + 2,
           input_size - 1);
    printf("%05lx-%05lx: 40 00         marks end of compressed image\n",
           input_pos, input_pos + 1);
  }

  /*
  There's a checksum element that has to be skipped along with some padding.
  */
  read_word();

  if (verbose) {
    printf("%05lx-%05lx: %02x %02x         checksum/size\n", input_pos,
           input_pos + 1, input_data[input_pos], input_data[input_pos + 1]);
  }

  if (current_byte() == 0xff) {
    count = 0;

    while (current_byte() == 0xff) {
      count++;
      read_byte();
    }

    if (verbose) {
      printf("%05lx-%05lx: ff ..         padding\n", input_pos,
             input_pos + count - 1);
    }
  }

  while (!finished) {
    switch (read_byte()) {
    case RLE_REPEAT_END:
      finished = 1;
    case RLE_COPY_END:
      finished = 1;

    case RLE_REPEAT:
      count = read_word();
      byte = read_byte();

      if (verbose) {
        printf("%05lx-%05lx: %02x %02x %02x %02x   repeat   ", input_pos,
               input_pos + 4, input_data[input_pos], input_data[input_pos + 1],
               input_data[input_pos + 2], input_data[input_pos + 3]);
      }

      while (count-- > 0) {
        write_byte(byte);
      }

      if (verbose) {
        printf("\n");
      }

      break;

    case RLE_COPY:
      count = read_word();

      if (verbose) {
        printf("%05lx-%05lx: .. %02x %02x %02x   copy     ", input_pos - count,
               input_pos + 2, input_data[input_pos], input_data[input_pos + 1],
               input_data[input_pos + 2]);
      }

      while (count-- > 0) {
        write_byte(read_byte());
      }

      if (verbose) {
        printf("\n");
      }

      break;

    default:
      error(10, "unexpected RLE packet 0x%02x\n", input_data[input_pos]);
    }
  }

  if (verbose) {
    printf("00000-%05lx: ..            crap\n", input_pos);
  }

  if (fwrite(output_data, output_size, 1, output_file) != 1) {
    error(10, "couldn't write output file\n");
  }

  error(0, "written ROM image to '%s'\n", output_name);
}
