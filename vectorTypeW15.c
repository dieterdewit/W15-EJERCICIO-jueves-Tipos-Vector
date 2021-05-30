/**
 * @author RookieHPC
 * @brief Original source code at https://www.rookiehpc.com/mpi/docs/mpi_type_vector.php
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * @brief Illustrates how to create a vector MPI datatype.
 * @details This program is meant to be run with 2 processes: a sender and a
 * receiver. These two MPI processes will exchange a message made of three
 * integers. On the sender, that message is in fact the middle column of an
 * array it holds, which will be represented by an MPI vector.
 *
 *
 *     Full array          What we want
 *                            to send
 * +-----+-----+-----+  +-----+-----+-----+
 * |  0  |  1  |  2  |  |  -  |  1  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  3  |  4  |  5  |  |  -  |  4  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  6  |  7  |  8  |  |  -  |  7  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 *
 * How to extract a column with a vector type:
 *
 *                 distance between the
 *            start of each block: 3 elements
 *          <---------------> <--------------->
 *         |                 |                 |
 *      start of          start of          start of
 *      block 1           block 2           block 3
 *         |                 |                 |
 *         V                 V                 V
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *   |  -  |  1  |  -  |  -  |  4  |  -  |  -  |  7  |  -  |
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *          <--->             <--->             <--->
 *         block 1           block 2           block 3
 *
 * Block length: 1 element
 * Element: MPI_INT
 **/
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    // Get the number of processes and check only 12 processes are used
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 12){
        printf("This application is meant to be run with 12 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Get my rank and do the corresponding job
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int global_max;
    int n = 144 / size;
    int received[n];

    if (my_rank == 0){
        int vec[12][12];
        for (int i = 0; i < 12; i++){
            for (int j = 0; j < 12; j++){
                vec[i][j] = rand() % 1001;
                printf("%d ", vec[i][j]);
            }
        }
        // Create the datatype
        MPI_Datatype column_type;
        MPI_Type_vector(size, 1, size, MPI_INT, &column_type);
        MPI_Type_commit(&column_type);

        // Send the message
        for (int i = 0; i < size; i++){
            MPI_Send(&vec[0][i], 1, column_type, i, 0, MPI_COMM_WORLD);
        }
        MPI_Recv(&received, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else{
        // Receive the message
        MPI_Recv(&received, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (my_rank == 0){
        printf("\nMAX: %d\n", global_max);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
