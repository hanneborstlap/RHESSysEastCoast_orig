/*--------------------------------------------------------------*/
/* 																*/
/*							world_daily_F							*/
/*																*/
/*	NAME														*/
/*	world_daily_F													*/
/*				 - performs cycling and output of a world		*/
/*					with parallel hillslope simulations			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void		world_daily_F(									*/
/*					long   	day,								*/
/*					struct world_object *world,					*/
/*					struct command_line_object *command_line,	*/
/*					struct tec_entry *event,					*/
/*					struct date current_date)					*/
/* 																*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	Performs daily simulation of the world with hillslope and	*/
/*	lower level executed in parallel.							*/
/*																*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	At the moment this routine only handles setting Io and 		*/
/*	declination for the world.  								*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void world_daily_F(
				   long   	day,
				   struct world_object *world,
				   struct command_line_object *command_line,
				   struct tec_entry *event,
				   struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/

	// READ IN INUNDATION FILES HERE 

	const char *depth_filename = "inundation/inundation_depth.txt";
    	const char *dur_filename = "inundation/inundation_dur.txt";
    	const char *date_filename = "inundation/inundation_date.txt";
    	const char *patchID_filename = "inundation/inundation_patchID.txt";
    
    	float* depths = NULL;
    	float* durs = NULL;
    	char** dates = NULL;
    	int* patchIDs = NULL;
    
    	// Call processInundationData and pass pointers to arrays
    	int count = readInundationDepths(depth_filename, dur_filename, date_filename, patchID_filename, &depths, &durs, &dates, &patchIDs);
	
	void	basin_daily_F(
		long	,
		struct world_object *,
		struct basin_object *,
		struct command_line_object *,
		struct tec_entry *,
		struct date, double *depths, int *durs, char **dates, int *patchIDs, int count);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	basin;
	/*--------------------------------------------------------------*/
	/*	Simulate over all of the basins.							*/
	/*--------------------------------------------------------------*/
  world[0].target_status = 1;
	for ( basin = 0; basin < world[0].num_basin_files; basin++ ){
		basin_daily_F(	day,
			world,
			world[0].basins[basin],
			command_line,
			event,
			current_date, 
			depths, durs, dates, patchIDs, count);
	}
	return;
} /*end world_daily_F.c*/
