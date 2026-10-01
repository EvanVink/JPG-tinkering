#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint16_t read_16(const unsigned char one, const unsigned char two){
    uint16_t value = ((uint16_t)one << 8) | two;
    return value;
}


static int make_combo(char path_one[], char path_two[]){

    size_t data_start = 0;
    size_t data_end = 0;
    size_t header_start = 0;
    size_t header_end = 0;


    FILE *file_one = fopen(path_one, "rb");
    fseek(file_one, 0, SEEK_END);
    long file_size_one = ftell(file_one);
    fseek(file_one, 0, SEEK_SET);

    if(file_size_one <= 0){
        fprintf(stderr, "EMPTY OR INVALID FILE ONE\n");
        fclose(file_one);
        return 1;
    }

    FILE *file_two = fopen(path_two, "rb");
    fseek(file_two, 0, SEEK_END);
    long file_size_two = ftell(file_two);
    fseek(file_two, 0, SEEK_SET);

    if(file_size_two <= 0){
        fprintf(stderr, "EMPTY OR INVALID FILE TWO\n");
        fclose(file_one);
        fclose(file_two);
        return 1;
    }
    

    uint8_t *buffer_one = malloc((size_t)file_size_one);
    uint8_t *buffer_two = malloc((size_t)file_size_two);
    
    if(!buffer_one){
        fprintf(stderr, "buffer 1 error");
        fclose(file_one);
        fclose(file_two);
        return 1;
    }
    if(!buffer_two){
        fprintf(stderr, "buffer 2 error");
        fclose(file_one);
        fclose(file_two);
        return 1;
    }


    size_t bytes_read_one = fread(buffer_one, 1, file_size_one, file_one);
    size_t bytes_read_two = fread(buffer_two, 1, file_size_two, file_two);

    fclose(file_one);

    fclose(file_two);

    for(size_t i = 0; i < bytes_read_one; i++){
        // if(buffer_one[i] == 0xFF && buffer_one[i + 1] == 0xD8 && buffer_one[i + 2] == 0xFF){
        //     printf("\nfound headers\n");
        //     size_t counter = 0;
        //     size_t j = i;

        //     while(j + 1 < bytes_read_one){
        //         if(buffer_one[j] == 0xFF && buffer_one[j + 1] == 0xDA){
        //             //printf("\n (length = %zu)\n", counter);
        //             //printf("\n %02X %02X", buffer[j], buffer[j + 1]);
        //             break;
        //         }

        //         headers[counter] = buffer_one[j];
        //         //printf("%02X ", buffer[j]);

        //         header_length++;
        //         counter++;
        //         j++;

        //     }
        // }

        if(buffer_one[i] == 0xFF && buffer_one[i + 1] == 0xD8 && buffer_one[i + 2] == 0xFF){
            header_start = i;
            break;
        }
    }
    for(size_t i = header_start + 1; i < bytes_read_one; i++){

        if(buffer_one[i] == 0xFF && buffer_one[i + 1] == 0xDA){
            header_end = i + 1;
            break;
        }


    }


    for(size_t i = 0; i < bytes_read_two; i++){

        if(buffer_two[i] == 0xFF && buffer_two[i + 1] == 0xDA){
            data_start = i;
            break;
        }
    }

    for(size_t i = data_start + 1; i < bytes_read_two; i++){

        if(buffer_two[i] == 0xFF && buffer_two[i + 1] == 0xD9){
            data_end = i + 1;
            break;
        }

        
        // if(buffer_two[i] == 0xFF && buffer_two[i + 1] == 0xDA){
        //     printf("\nfound SOS\n");
            

            // size_t counter = 0;
            // size_t j = i;

            // while(j + 1 < bytes_read_two){
            //     img_data_buffer[counter] = buffer_two[j];
            //     //printf("%02X ", buffer[j]);

            //     if(buffer_two[j] == 0xFF && buffer_two[j + 1] == 0xD9){
            //         img_data_buffer[counter + 1] = buffer_two[j + 1];
            //         //printf("%02X ", buffer[j + 1]);
            //         image_data_length += 2;
            //         counter += 2;
            //         //printf("\n -> end of scan (length = %zu)\n", counter);
            //         break;
            //     }
            //     image_data_length++;
            //     counter++;
            //     j++;
            // }
        // }


    }

    size_t h = header_end - header_start;
    size_t d = data_end - data_start;

    printf("%d\n", h);
    printf("%d\n", d);



    size_t total_len = d + h;
    printf("%d", total_len);

    uint8_t *new_file_buffer = malloc(total_len);

    if(!new_file_buffer){
        fprintf(stderr, "malloc failed\n");
        free(new_file_buffer);
        return 1;
    }

    memcpy(new_file_buffer, buffer_one + header_start, h);
    memcpy(new_file_buffer + h, buffer_two + data_start, d);

    // memcpy(new_file_buffer, headers, header_length);
    // memcpy(new_file_buffer + header_length, img_data_buffer, image_data_length);

    FILE *newFile = fopen("./photo/bad.jpg", "wb");
    if (!newFile) {
        perror("Failed to create bad.jpg");
        // free everything...
        return 1;
    }

    fwrite(new_file_buffer, 1, total_len, newFile);
    

    fclose(newFile);
    free(new_file_buffer);
    free(buffer_one);
    free(buffer_two);

    return 0;
}





int main(int argc, char *argv[]){

    if(argc != 3){
        printf("USAGE: %s <filename.jpg> <filename.jpg>", argv[0]);
        return 1;
    }


    int num = make_combo(argv[1], argv[2]);

    // size_t header_length = 0;
    // size_t image_data_length = 0;

    
    // FILE *file = fopen("./photo/red.jpg", "rb");
    // FILE *fileTwo = fopen("./photo/blue.jpg", "rb");

    // fseek(fileTwo, 0, SEEK_END);
    // long file_size_two = ftell(fileTwo);
    // fseek(fileTwo, 0, SEEK_SET);

    // if(file_size_two <= 0){
    //     fprintf(stderr, "Empty or invalid file\n");
    //     fclose(fileTwo);
    //     return 1;
    // }

    // FILE *newFile = fopen("./photo/bad.jpg", "wb");

    // fseek(file, 0, SEEK_END);
    // long file_size = ftell(file);
    // fseek(file, 0, SEEK_SET);

    // if(file_size <= 0){
    //     fprintf(stderr, "Empty or invalid file\n");
    //     fclose(file);
    //     return 1;
    // }

    // uint8_t *buffer = malloc(file_size);
    // uint8_t *img_data_buffer = calloc(file_size, sizeof(uint8_t));
    // uint8_t *headers = calloc(file_size, sizeof(uint8_t));

    // if(!buffer){
    //     fprintf(stderr, "buffer error");
    //     fclose(file);
    //     return 1;
    // }

    // if(!img_data_buffer){
    //     fprintf(stderr, "img buffer error");
    //     fclose(file);
    //     return 1;
    // }

    // if(!headers){
    //     fprintf(stderr, "headers buffer error");
    //     fclose(file);
    //     return 1;
    // }

    // size_t bytes_read = fread(buffer, 1, file_size, file);

    // fclose(file);

    // for(size_t i = 0; i < bytes_read; i++){
    //     //printf("%02X ", buffer[i]);

    //     if(buffer[i] == 0xFF && buffer[i + 1] == 0xC0){
    //         int height = read_16(buffer[i + 5], buffer[i + 6]);
    //         int width = read_16(buffer[i + 7], buffer[i + 8]);

    //         //printf("this image is %d x %d", width, height);
    //     }

    //     if(buffer[i] == 0xFF && buffer[i + 1] == 0xD8 && buffer[i + 2] == 0xFF){
    //         printf("\nfound headers\n");
    //         size_t counter = 0;
    //         size_t j = i;

    //         while(j + 1 < bytes_read){
    //             if(buffer[j] == 0xFF && buffer[j + 1] == 0xDA){
    //                 //printf("\n -> end of scan (length = %zu)\n", counter);
    //                 //printf("\n %02X %02X", buffer[j], buffer[j + 1]);
    //                 break;
    //             }

    //             headers[counter] = buffer[j];
    //             //printf("%02X ", buffer[j]);

    //             header_length++;
    //             counter++;
    //             j++;

    //         }
    //     }



    //     if(buffer[i] == 0xFF && buffer[i + 1] == 0xDA){
    //         printf("\nfound SOS\n");
            

    //         size_t counter = 0;
    //         size_t j = i;

    //         while(j + 1 < bytes_read){
    //             img_data_buffer[counter] = buffer[j];
    //             //printf("%02X ", buffer[j]);

    //             if(buffer[j] == 0xFF && buffer[j + 1] == 0xD9){
    //                 img_data_buffer[counter + 1] = buffer[j + 1];
    //                 //printf("%02X ", buffer[j + 1]);
    //                 image_data_length += 2;
    //                 counter += 2;
    //                 //printf("\n -> end of scan (length = %zu)\n", counter);
    //                 break;
    //             }
    //             image_data_length++;
    //             counter++;
    //             j++;
    //         }


    //     }
    // }

    // size_t total_len = header_length + image_data_length;
    // printf("%d", total_len);

    // uint8_t *new_file_buffer = malloc(total_len);

    // if(!new_file_buffer){
    //     fprintf(stderr, "malloc failed\n");
    //     free(new_file_buffer);
    //     return 1;
    // }

    // memcpy(new_file_buffer, headers, header_length);
    // memcpy(new_file_buffer + header_length, img_data_buffer, image_data_length);

    // fwrite(new_file_buffer, 1, total_len, newFile);

    // fclose(newFile);
    
    // free(headers);
    // free(new_file_buffer);
    // free(img_data_buffer);
    // free(buffer);



    return 0;
}