/*
Looking Glass - KVM FrameRelay (KVMFR) Client
Copyright (C) 2017-2019 Geoffrey McRae <geoff@hostfission.com>
https://looking-glass.hostfission.com

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "app.h"

#include <stdio.h>
#include <windows.h>
#include "debug.h"
#include "capture/interfaces.h"

int app_main()
{
  unsigned int shmemSize = os_shmemSize();
  void * shmemMap = NULL;

  DEBUG_INFO("IVSHMEM Size     : %u MiB", shmemSize / 1048576);
  if (!os_shmemMmap(&shmemMap) || !shmemMap)
  {
    DEBUG_ERROR("Failed to map the shared memory");
    return -1;
  }

  struct CaptureInterface * iface = NULL;
  for(int i = 0; CaptureInterfaces[i]; ++i)
  {
    iface = CaptureInterfaces[i];
    DEBUG_INFO("Trying           : %s", iface->getName());
    if (!iface->create())
      continue;

    if (iface->init())
      break;

    iface->free();
    iface = NULL;
  }

  if (!iface)
  {
    DEBUG_ERROR("Failed to find a supported capture interface");
    return -1;
  }

  DEBUG_INFO("Using            : %s", iface->getName());

  iface->capture();
  iface->capture();
  iface->capture();

  iface->deinit();
  iface->free();
  return 0;
}