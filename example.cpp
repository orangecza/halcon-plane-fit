#ifndef __APPLE__
#include "HDevThread.h"
#include "HalconCpp.h"
#else
#ifndef HC_LARGE_IMAGES
#include <HALCON/HpThread.h>
#include <HALCONCpp/HDevThread.h>
#include <HALCONCpp/HalconCpp.h>
#else
#include <HALCONCppxl/HDevThread.h>
#include <HALCONCppxl/HalconCpp.h>
#include <HALCONxl/HpThread.h>
#endif
#include <CoreFoundation/CFRunLoop.h>
#include <stdio.h>
#endif

#include "FilterRefit.hpp"
#include "GenFilterModel3D.hpp"

using namespace HalconCpp;

#ifndef NO_EXPORT_MAIN
// Main procedure
void
action ()
{

  // Local iconic variables
  HObject ho_img;

  // Local control variables
  HTuple hv_DeviDiff, hv_MeanDiff, hv_i, hv_ObjectModel3D;
  HTuple hv_ObjectModel3D1, hv_ObjectModel3D2, hv_ObjectModel3D3;
  HTuple hv_UnionObjectModel3D, hv_X, hv_Y, hv_Z, hv_Plane;
  HTuple hv_ObjectModel3D4, hv_Pose, hv_Dist, hv_Deviation;
  HTuple hv_Mean, hv_FileHandle, hv_Index;

  hv_DeviDiff = HTuple ();
  hv_MeanDiff = HTuple ();
  for (hv_i = 1; hv_i <= 100; hv_i += 1)
    {
      ReadImage (
          &ho_img,
          ("imgdata-exposure-diff-100/out"
           + hv_i)
              + ".tiff");
      GenFilterModel3D (ho_img, 615, 685, 15, 15, 1.5708, 0.01, 2000, 0.1,
                        &hv_ObjectModel3D);
      GenFilterModel3D (ho_img, 715, 695, 15, 15, 1.5708, 0.01, 2000, 0.1,
                        &hv_ObjectModel3D1);
      GenFilterModel3D (ho_img, 615, 730, 15, 15, 1.5708, 0.01, 2000, 0.1,
                        &hv_ObjectModel3D2);
      GenFilterModel3D (ho_img, 715, 745, 15, 15, 1.5708, 0.01, 2000, 0.1,
                        &hv_ObjectModel3D3);
      UnionObjectModel3d (((hv_ObjectModel3D.TupleConcat (hv_ObjectModel3D1))
                               .TupleConcat (hv_ObjectModel3D2))
                              .TupleConcat (hv_ObjectModel3D3),
                          "points_surface", &hv_UnionObjectModel3D);
      GetObjectModel3dParams (hv_UnionObjectModel3D, "point_coord_x", &hv_X);
      GetObjectModel3dParams (hv_UnionObjectModel3D, "point_coord_y", &hv_Y);
      GetObjectModel3dParams (hv_UnionObjectModel3D, "point_coord_z", &hv_Z);
      FilterRefit (hv_UnionObjectModel3D, 0., 0.15, &hv_Plane);

      GenFilterModel3D (ho_img, 530, 715, 15, 15, 1.5708, 0.01, 2000, 0.1,
                        &hv_ObjectModel3D4);
      // GenFilterModel3D (img, 315, 1380, 100, 100, 1.5708, 0.01, 2000, 0.25,
      // ObjectModel3D4) get_object_model_3d_params (Plane,
      // 'primitive_parameter', PlanePara)
      GetObjectModel3dParams (hv_Plane, "primitive_parameter_pose", &hv_Pose);
      GenPlaneObjectModel3d (hv_Pose, HTuple (), HTuple (), &hv_Plane);
      DistanceObjectModel3d (hv_ObjectModel3D4, hv_Plane, HTuple (), 0,
                             "signed_distances", "false");
      GetObjectModel3dParams (hv_ObjectModel3D4, "&distance", &hv_Dist);

      TupleDeviation (hv_Dist, &hv_Deviation);
      TupleConcat (hv_Deviation, hv_DeviDiff, &hv_DeviDiff);
      TupleMean (hv_Dist, &hv_Mean);
      TupleConcat (hv_Mean, hv_MeanDiff, &hv_MeanDiff);
    }

  OpenFile ("re.csv", "append", &hv_FileHandle);
  {
    HTuple end_val29 = (hv_DeviDiff.TupleLength ()) - 1;
    HTuple step_val29 = 1;
    for (hv_Index = 0; hv_Index.Continue (end_val29, step_val29);
         hv_Index += step_val29)
      {
        FwriteString (hv_FileHandle,
                      HTuple (hv_DeviDiff[hv_Index]) + HTuple (","));
      }
  }
  FnewLine (hv_FileHandle);
  {
    HTuple end_val33 = (hv_MeanDiff.TupleLength ()) - 1;
    HTuple step_val33 = 1;
    for (hv_Index = 0; hv_Index.Continue (end_val33, step_val33);
         hv_Index += step_val33)
      {
        FwriteString (hv_FileHandle,
                      HTuple (hv_MeanDiff[hv_Index]) + HTuple (","));
      }
  }
  FnewLine (hv_FileHandle);
}

#ifndef NO_EXPORT_APP_MAIN

#ifdef __APPLE__
// On OS X systems, we must have a CFRunLoop running on the main thread in
// order for the HALCON graphics operators to work correctly, and run the
// action function in a separate thread. A CFRunLoopTimer is used to make sure
// the action function is not called before the CFRunLoop is running.
// Note that starting with macOS 10.12, the run loop may be stopped when a
// window is closed, so we need to put the call to CFRunLoopRun() into a loop
// of its own.
static HMutex *sStartMutex;
static H_pthread_t sActionThread;
static bool sTerminate = false;

static void
timer_callback (CFRunLoopTimerRef timer, void *info)
{
  sStartMutex->UnlockMutex ();
}

static Herror
apple_action (void **parameters)
{
  // Wait until the timer has fired to start processing.
  sStartMutex->LockMutex ();
  sStartMutex->UnlockMutex ();

  try
    {
      action ();
    }
  catch (HException &exception)
    {
      fprintf (stderr, "  Error #%u in %s: %s\n", exception.ErrorCode (),
               exception.ProcName ().TextA (),
               exception.ErrorMessage ().TextA ());
    }

  // Tell the main thread to terminate itself.
  sStartMutex->LockMutex ();
  sTerminate = true;
  sStartMutex->UnlockMutex ();
  CFRunLoopStop (CFRunLoopGetMain ());
  return H_MSG_OK;
}

static int
apple_main (int argc, char *argv[])
{
  Herror error;
  CFRunLoopTimerRef Timer;
  CFRunLoopTimerContext TimerContext = { 0, 0, 0, 0, 0 };

  sStartMutex = new HMutex ("type", "sleep");
  sStartMutex->LockMutex ();

  error = HpThreadHandleAlloc (&sActionThread);
  if (H_MSG_OK != error)
    {
      fprintf (stderr, "HpThreadHandleAlloc failed: %d\n", error);
      exit (1);
    }

  error = HpThreadCreate (sActionThread, 0, apple_action);
  if (H_MSG_OK != error)
    {
      fprintf (stderr, "HpThreadCreate failed: %d\n", error);
      exit (1);
    }

  Timer
      = CFRunLoopTimerCreate (kCFAllocatorDefault, CFAbsoluteTimeGetCurrent (),
                              0, 0, 0, timer_callback, &TimerContext);
  if (!Timer)
    {
      fprintf (stderr, "CFRunLoopTimerCreate failed\n");
      exit (1);
    }
  CFRunLoopAddTimer (CFRunLoopGetCurrent (), Timer, kCFRunLoopCommonModes);

  for (;;)
    {
      bool terminate;

      CFRunLoopRun ();

      sStartMutex->LockMutex ();
      terminate = sTerminate;
      sStartMutex->UnlockMutex ();

      if (terminate)
        break;
    }

  CFRunLoopRemoveTimer (CFRunLoopGetCurrent (), Timer, kCFRunLoopCommonModes);
  CFRelease (Timer);

  error = HpThreadHandleFree (sActionThread);
  if (H_MSG_OK != error)
    {
      fprintf (stderr, "HpThreadHandleFree failed: %d\n", error);
      exit (1);
    }

  delete sStartMutex;
  return 0;
}
#endif

int
main (int argc, char *argv[])
{
  int ret = 0;

  try
    {
#if defined(_WIN32)
      SetSystem ("use_window_thread", "true");
#endif

      // Default settings used in HDevelop (can be omitted)
      SetSystem ("width", 512);
      SetSystem ("height", 512);

#ifndef __APPLE__
      action ();
#else
      ret = apple_main (argc, argv);
#endif
    }
  catch (HException &exception)
    {
      fprintf (stderr, "  Error #%u in %s: %s\n", exception.ErrorCode (),
               exception.ProcName ().TextA (),
               exception.ErrorMessage ().TextA ());
      ret = 1;
    }
  return ret;
}

#endif

#endif
