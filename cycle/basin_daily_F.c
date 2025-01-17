/*--------------------------------------------------------------*/
/* 																*/
/*						basin_daily_F							*/
/*																*/
/*	NAME														*/
/*	basin_daily_F												*/
/*			 - performs cycling and output of a basin			*/
/*				for beginning of the  day 						*/
/*				with hillslopes simulated in parallel			*/
/*																*/
/*																*/
/*	SYNOPSIS													*/
/*	void basin_daily_F( 										*/
/*						 long	,								*/
/*						 struct world_object *,					*/
/*						 struct basin_object *,					*/
/*						 struct command_line_object *,			*/
/*						 struct tec_entry *,					*/
/*						 struct date)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	This routine performs simulation cycles on an identified	*/
/*	hillslopes in the basin. The routine also prints out results*/
/*	where specified by current tec events files.				*/
/*																*/
/*	Computes day length at the latitude of the basin.			*/
/*	Executes hillslope daily simulation for all hillslopes. 	*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*																*/
/*	The execution of simulate_hillslope_daily could be done in 	*/
/*	parallel with one processor per hillslope.  				*/
/*																*/
/*	If message passing is required during the simulate_hillslope*/
/*	_daily processes a message structure could be created in 	*/
/*	the basin_daily array.										*/
/*																*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"

void	basin_daily_F(
					  long	day,
					  struct	world_object	*world,
					  struct	basin_object 	*basin,
					  struct	command_line_object *command_line,
					  struct	tec_entry		*event,
					  struct	date			current_date)
{
	/*--------------------------------------------------------------*/
	/*  Local Function Declarations.                                */
	/*--------------------------------------------------------------*/
	void hillslope_daily_F(
		long,
		struct	world_object *,
		struct 	basin_object *,
		struct	hillslope_object *,
		struct	command_line_object *, 
		struct	tec_entry *,
		struct	date );
	/*--------------------------------------------------------------*/
	/* this part has been moved to basin_hourly			*/
	/*--------------------------------------------------------------*/
	void	compute_subsurface_routing(
		struct command_line_object *,
		struct basin_object *,
		int, struct	date);
	
	double	compute_stream_routing(
		struct command_line_object *,
		struct stream_network_object *,
		int, struct	date);

	void	update_basin_patch_accumulator(
		struct command_line_object *command_line,
		struct basin_object *basin,
		struct date current_date);

	void	update_hillslope_accumulator(
		struct command_line_object *command_line,
		struct basin_object *basin);
	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int	h, z, p,inx;
	double	scale;
	double aarea; 
	struct	hillslope_object *hillslope;
	struct	zone_object *zone;
	struct	patch_object *patch; 
	struct	dated_sequence	clim_event;

	/*--------------------------------------------------------------*/
	/* 	track basin scale snow for snow assimilation 			*/
	/*--------------------------------------------------------------*/
	basin[0].area_withsnow=0;
	basin[0].snowpack.surface_age = 0.0;
	basin[0].snowpack.energy_deficit = 0.0;
	basin[0].snowpack.T = 0.0;

	/*--------------------------------------------------------------*/
	/*	Calculate ditch routing parameter based on calibration and current basin wide saturation deficit 	*/	
	/*--------------------------------------------------------------*/
	for (h=0; h < basin[0].num_hillslopes; h++){
		hillslope = basin[0].hillslopes[h];

		for (z=0; z< hillslope[0].num_zones; z++){
			zone = hillslope[0].zones[z];
			
			for (p=0; p< zone[0].num_patches; p++){
				patch = zone[0].patches[p];
				basin[0].sat_deficit += patch[0].sat_deficit * patch[0].area;
				// these next variables don't vary in time, so could define those earlier so they don't have to be recalculated every time?
				// basin[0].sat_min = patch[0].soil_defaults[0][0].soil_water_cap * patch[0].area;
				basin[0].sat_min = 0;
				basin[0].sat_max = patch[0].soil_defaults[0][0].soil_water_cap * patch[0].area;
				aarea +=  patch[0].area;
			}

		}
	}
		
		basin[0].sat_deficit /= aarea;
		basin[0].sat_min /= aarea; 
		basin[0].sat_max /= aarea; 

		// Normalized saturation deficit [N = (X_current - X_min)/(X_max - X_min)]
		basin[0].N_satdef = (basin[0].sat_deficit-basin[0].sat_min)/(basin[0].sat_max-basin[0].sat_min); 

		// The eventual parameter of ditch routing is a function of saturation deficit. With drier conditions, 
	        // there will be more water infiltration along the flowpath (lower ditch_routing). During wetter conditions, there will be 
		// more water routed directly to the watershed outlet (higher ditch_routing).

		// Defined in Patch_daily_F.c and Output_24hrs_basin.c; 
		// patch[0].detention_store += patch[0].ditch_extraction*(1-ditch_routing)
		// basin[0].streamflow += patch[0].streamflow*ditch_routing
	
		basin[0].ditch_routing = command_line[0].ditch_routing*(1-basin[0].N_satdef);
	
	/*--------------------------------------------------------------*/
	/*	Simulate the hillslopes in this basin for the whole day		*/
	/*--------------------------------------------------------------*/

	for ( h = 0 ; h < basin[0].num_hillslopes; h ++ ){
		hillslope_daily_F(	day,
			world,
			basin,
			basin[0].hillslopes[h],
			command_line, 
			event,
			current_date );
	}

        hillslope = basin[0].hillslopes[0];
	zone = hillslope[0].zones[0];
	basin[0].snowpack.surface_age /=  basin[0].area_withsnow;
	basin[0].snowpack.T /=  basin[0].area_withsnow;
	basin[0].snowpack.energy_deficit /=  basin[0].area_withsnow;


	/*--------------------------------------------------------------*/
	/*  For routing option - route water between patches within     */
	/*      the basin:  this part has been moved to basin_hourly    */
	/*--------------------------------------------------------------*/
   	if ( command_line[0].routing_flag == 1 && zone[0].hourly_rain_flag == 0) {
		compute_subsurface_routing(command_line,
			basin,
			basin[0].defaults[0][0].n_routing_timesteps,
			current_date);
	}
	
	/*--------------------------------------------------------------*/
	/*  For stream routing option - route water between patches within     */
	/*      the basin                                               */
	/*--------------------------------------------------------------*/
    	if ( command_line[0].stream_routing_flag == 1) {
		 basin[0].stream_list.streamflow=compute_stream_routing(command_line,
			basin[0].stream_list.stream_network,
			basin[0].stream_list.num_reaches,
                        current_date);
	}

	/*--------------------------------------------------------------*/
	/* update basin patch accumulator				*/
	/*--------------------------------------------------------------*/
    if(command_line[0].output_flags.yearly == 1 || command_line[0].output_flags.monthly == 1) update_basin_patch_accumulator(command_line,
                    basin,
                    current_date);

	/*--------------------------------------------------------------*/
	/* update hillslope accumulator					*/
	/*--------------------------------------------------------------*/
//    update_hillslope_accumulator(command_line,
//                    basin);

	return;
} /*end basin_daily_F*/
