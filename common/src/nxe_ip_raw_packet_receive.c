/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Component                                                        */
/**                                                                       */
/**   Internet Protocol (IP)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_packet.h"
#include "nx_ip.h"

/* Bring in externs for caller checking code.  */

NX_CALLER_CHECKING_EXTERNS


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nxe_ip_raw_packet_receive                          PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the IP raw packet receive        */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                            IP control block pointer          */
/*    packet_ptr                        Pointer to place allocated packet */
/*                                        in                              */
/*    wait_option                       Suspension option                 */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                            Completion status                 */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_packet_release                    Release invalid packet        */
/*    _nx_ip_raw_packet_receive             Actual IP raw packet receive  */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _nxe_ip_raw_packet_receive(NX_IP *ip_ptr, NX_PACKET **packet_ptr,
                                 ULONG wait_option)
{

UINT status;


    /* Check for invalid input pointers.  */
    if ((ip_ptr == NX_NULL) || (ip_ptr -> nx_ip_id != NX_IP_ID) || (packet_ptr == NX_NULL))
    {
        return(NX_PTR_ERROR);
    }

    /* Check to see if IP raw packet processing is enabled.  */
    if (!ip_ptr -> nx_ip_raw_ip_processing)
    {
        return(NX_NOT_ENABLED);
    }

    /* Check for appropriate caller.  */
    NX_THREADS_ONLY_CALLER_CHECKING

    do
    {

        /* Call actual IP raw packet receive function.  */
        status =  _nx_ip_raw_packet_receive(ip_ptr, packet_ptr, wait_option);

        /* Check for good status.  */
        if (status == NX_SUCCESS)
        {

            /* Check for an invalid packet prepend pointer.  */
            if ((((*packet_ptr) -> nx_packet_prepend_ptr - (sizeof(NX_IP_HEADER))) < (*packet_ptr) -> nx_packet_data_start) ||
                ((*packet_ptr) -> nx_packet_append_ptr > (*packet_ptr) -> nx_packet_data_end))
            {

                /* Release the packet.  */
                _nx_packet_release(*packet_ptr);

                /* Clear packet pointer.  */
                *packet_ptr =  NX_NULL;

                /* Set the status to an error.  */
                status =  NX_PTR_ERROR;
            }
        }
    } while ((status != NX_SUCCESS) && (status != NX_NO_PACKET));

    /* Return completion status.  */
    return(status);
}

