/*
 * Utility to send chip reset ioctl to mpt_sas driver on an illumos 
 * platform.
 *
 * Originally from J. Molanus
 *
 * Type in the command with no parameter and it will give
 * you the correct devhdl device path. Then type command
 * again with the suggested device path.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "mpi2_type.h"
#include "mpi2.h"
#include "mpi2_init.h"
#include "mptsas_ioctl.h"
#include <assert.h>

#include <errno.h>
#include <libdevice.h>
#include <libdevinfo.h>

static int
proces_node(di_node_t d, di_minor_t dm, void *arg)
{

    char *name;

    if ((name = di_devfs_path(d)) != NULL) {

            char *dname = di_driver_name(d);

            if ((strcmp(dname, "mpt_sas") == 0)  &&
                (strstr(name, "iport") == NULL)) {
                    printf("%s%d : /devices%s:devctl\n", dname, di_instance(d),
                       name);
            }
    }

}
void
list_mptsas()
{
    di_node_t d;

    d = di_init("/", DINFOSUBTREE | DINFOMINOR);
    assert (d != NULL);

    di_walk_minor(d, DDI_NT_NEXUS, NULL, 0, &proces_node);
    di_fini(d);

    exit(0);
}

int main(int argc, char *argv[])
{

    int i = 0;
    int fd;

    if (argc < 2) {
            printf("need mptsas card as argument\n");
                list_mptsas();
                return (-1);

    }

    if ((fd = open(argv[1], O_RDWR)) == -1) {
        printf("Failed to open %s : %s", argv[1], strerror(errno));
        return (-1);
    }

    if (ioctl(fd, (MPTIOCTL | 3), NULL) == -1) {
        printf("reset failed %s", strerror(errno));

        return (-1);
    }

    if (fd >0)
        close(fd);
    return (0);
}
