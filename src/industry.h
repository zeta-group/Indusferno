/**
 * @file industry.h
 * @author Gustavo Rehermann (rehermann6046@gmail.com)
 * @brief Industries.
 * @version added in 0.1
 * @date 2021-03-11
 *
 * @copyright Copyright (c)Gustavo Ramos Rehermann 2021. The MIT License.
 */

#ifndef INDUSTRY_H
#define INDUSTRY_H


#include <limits.h>
#include <stddef.h>
#include <stdio.h>


#define MAX_INDUS_MATS  5
#define MAX_INDUS_TYPES 32
#define MAX_INDUSTRIES  128


/**
 * @brief An industry supply type.
 * 
 * Each industry type can produce supply cargo in a number of ways.
 */
enum industry_supply_type_t {
    /**
     * @brief Zero. Denotes an invalid industry type.
     */
    ISUPTYPE_UNKNOWN,

    /**
     * @brief Boost-type industries.
     * 
     * Those always produce at least their base production
     * rate, but can be boosted by supplying them with enough
     * of the accepted cargo amount over the current period.
     */
    ISUPTYPE_BOOST,

    /**
     * @brief Assemble-type industries
     * 
     * Those only produce if all of the accepted cargo types are
     * supplied to them, and always produce by the lowest common
     * denominator of the accepted cargo types.
     * 
     * They cannot be boosted.
     */
    ISUPTYPE_ASSEMBLE,

    /**
     * @brief Convert-type industries.
     * 
     * Those produce if any of at least one of the accepted cargo
     * types are supplied to them. If all cargo types are supplied,
     * they are boosted, which multiplies the output production.
     */
    ISUPTYPE_CONVERT
};

/**
 * @brief An industry type.
 */
struct industry_type_t {
    /**
     * @brief The supply type of this industry.
     * 
     * @see industry_supply_type_t
     */
    enum industry_supply_type_t supply_type;

    /**
     * @brief The label of this industry type.
     * 
     * A human-friendly label that gives industries of this
     * type a common name.
     */
    char label[64];

    /**
     * @brief The industry spawner type.
     * 
     * The actor class name to be spawned in the position of this
     * industry.
     * 
     * This makes the industry both visible and collidable in the
     * physical world, but are otherwise only a way to physically
     * visualize the position of an industry. Everything else is
     * handled in the code.
     */
    char spawner_type[64];

    /**
     * @brief The base production rate.
     * 
     * For boost-type industries. this is the amount of cargo that is
     * always produced over a period's length of time. This can still
     * be boosted by boost_rate.
     */
    float base_production;

    /**
     * @brief The boost rate.
     * 
     * How much the output production is multiplied by, if an industry
     * of this type is 'boosted'.
     */
    float boost_rate;

    /**
     * @brief The boost threshold.
     * 
     * For boost-type industries, how much material must be received in
     * Material Units to trigger the boost-state of industries of this
     * type.
     */
    float boost_threshold;

    /**
     * @brief The radius of station reach.
     * 
     * How far a station can be in order for matching cargo unloaded in
     * it to be supplied into this industry, as well as for this
     * industry to consider moving its produced supply cargo there
     * if the station is flagged as accepting that type of cargo.
     */
    float reach;

    /**
     * @brief Cargo types accepted, by index.
     * 
     * If less than MAX_INDUS_MATS values are present, -1 must be set
     * after the last one, as a sort of terminator, to denote the end
     * of the list.
     */
    size_t accepts[MAX_INDUS_MATS];

    /**
     * @brief Weight of accepted cargo, by index.
     * 
     * Any cargo supplied to this industry of an accepted type may
     * result in said industry attaining a certain amount of Material
     * Units.
     * 
     * The amount of material generated by supplying an accepted cargo
     * is the amount of accepted cargo in Cargo Units, multiplied by
     * the *weight* of that accepted cargo.
     */
    float accept_weight[MAX_INDUS_MATS];

    /**
     * @brief Cargo types supplied, by index.
     * 
     * If less than MAX_INDUS_MATS values are present, -1 must be set
     * after the last one, as a sort of terminator, to denote the end
     * of the list.
     */
    size_t supplies[MAX_INDUS_MATS];

    /**
     * @brief Weight of supplied cargo, by index.
     * 
     * Any cargo that this industry supplies stems from the production
     * generated by processing the material accumulated in this
     * industry.
     * 
     * The precise amount of cargo generated, in Cargo Units, is the
     * amount of production generated, in Production Units, multiplied
     * by each cargo type's corresponding supply weight.
     * 
     * Production is not split by the number of cargo types supplied.
     */
    float supply_weight[MAX_INDUS_MATS];
};

/**
 * @brief An instance of an industry somewhere in the world.
 */
struct industry_t {
    /**
     * @brief The index of this industry's type.
     *
     * Every industry is of a corresponding industry type.
     *
     * This is an index into industry_types.
     */
    size_t type;

    /**
     * @brief All material accumulated in this industry.
     *
     * All material accumulated in this industry in Material
     * Units, grouped by cargo type.
     *
     * Each item's cargo type is defined by the industry type's
     * corresponding 'accepts' item.
     */
    float material[MAX_INDUS_MATS];

    /**
     * @brief Total of all material accumulated in this industry.
     *
     * The ungrouped total of all material accumulated in this industry,
     * in Material Units.
     */
    float material_tot;

    /**
     * @brief The X and Y position of this industry in the world.
     */
    float pos_x, pos_y;

    // -- Stats

    /**
     * @brief The produced amount of supplied cargo over this period.
     *
     * The produced amount of each supplied cargo type in the current period,
     * in Cargo Units.
     *
     * Each number is reset at the end of the period.
     */
    float produced[MAX_INDUS_MATS];

    /**
     * @brief The transported ratio of supplied cargo over this period.
     *
     * The relative amount of each supplied cargo type that was
     * transported (i.e. distributed into a station) in the current
     * period.
     *
     * 1.0 means it was all transported from reachable stations.
     *
     * All values here are reset at the end of the period.
     */
    float transported[MAX_INDUS_MATS];
};

/**
 * @brief The list of all industry types defined.
 */
extern const struct industry_type_t industry_types[MAX_INDUS_TYPES];

/**
 * @brief The list of all industries in the world.
 */
extern struct industry_t industries[MAX_INDUSTRIES];

/**
 * @brief The number of all industries in the world.
 */
extern int num_industries;


// --

/**
 * @brief Checks if an industry is boosted in its current state.
 * 
 * @param indus The industry whose state to be checked for boosting.
 * @param indtype The type of the industry 'indus'.
 * @return unsigned char 1 if this industry is boosted, else 0.
 */
unsigned char industry_is_boosted(struct industry_t *const indus, const struct industry_type_t *const indtype);

/**
 * @brief Produce a certain amount of production units from this industry.
 * 
 * Each supplied cargo type is distributed evenly to any reachable stations where
 * that cargo type is loaded. The exact amount of each cargo is the amount of
 * production units multiplied by the respective supply_weight value in that industry's
 * type. Each is divided by the number of stations loading that kind of cargo.
 * 
 * @param indus The industry from the which to make production.
 * @param indtype The type of that industry from the which to make production.
 * @param amount The amount of production units to be converted into cargo units.
 */
void industry_make_production(struct industry_t *const indus, const struct industry_type_t *const indtype, float amount);

/**
 * @brief Update an industry to convert any (and all) accumulatedmaterial into production.
 * 
 * Checks if this industry in its current state has any production to be made,
 * and if so, makes it, taking boost level into account and spending accuumulated materials.
 * 
 * @param indus 
 * @param indtype 
 */
void industry_check_production(struct industry_t *const indus, const struct industry_type_t *const indtype);

/**
 * @brief Accepts into an industry a specific type of accepted cargo, at a specific amount.
 * 
 * @param ind_industry Index of the industry instance.
 * @param ind_accept Index of the accepted cargo in the industry's type. NOT cargo type!
 * @param amount Amount of this cargo to be accepted.
 */
void industry_accept_cargo(size_t ind_industry, size_t ind_accept, float amount);

// TODO: decide on a way to do industry_spawn
//size_t industry_spawn(size_t ind_indus_type, );


#endif // INDUSTRY_H