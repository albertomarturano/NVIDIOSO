//
//  constraint.h
//  NVIDIOSO
//
//  Created by Federico Campeotto on 08/07/14.
//  Copyright (c) 2014 ___UDNMSU___. All rights reserved.
//
//  This class represents the interface/abstract class for all constraints.
//  Defines how to construct a constraint, impose, check satisiability,
//  enforce consistency, etc.
//  Specific implementations based on hardware/software capabilities (e.g., CUDA)
//  should derive from this class.
//

#ifndef NVIDIOSO_constraint_h
#define NVIDIOSO_constraint_h

#include "globals.h"
#include "variable.h"

class Constraint;
typedef std::shared_ptr<Constraint> ConstraintPtr;

enum class ConsistencyType {
  BOUND_C,
  DOMAIN_C,
  OTHER_C
};

class Constraint : public std::enable_shared_from_this< Constraint > {
private:
  //! Unique global identifier for a given constraint.
  size_t _unique_id;
  
  /**
   * It specifies the weight of this constraint.
   * Weight can be used to implement soft constraints or imposing
   * an order on the propagation of constraints.
   * Default value is 0.
   */
  int _weight;
  
protected:
  //! Debug string
  std::string _dbg;
  
  /**
   * It specifies the number if for a given constraint.
   * All constraints within the same type have unique number ids.
   */
  int _number_id;
  
  /**
   * It specifies the string id of the constraint.
   * If it is null, then the string id is created from string
   * associated for the constraint type and the _number_id
   * of the constraint.
   */
  std::string _str_id;
  
  /**
   * It specifies which kind of consistency the constraint
   * must ensure. 
   * There are at least two types of consistency:
   * 1 - bound consistency 
   * 2 - domain consistency
   * Default is bound consistency.
   */
  ConsistencyType _consistency;

  /**
   * It specifies the events which trigger the propagation
   * of a given constraint.
   * @note see domain.h for the list of events of type "EventType".
   */
  std::vector<EventType> _trigger_events;
  
  /**
   * It represents the array of auxiliary arguments needed by
   * a given constraint in order to be propagated.
   * For example:
   *    int_eq ( x, 2 ) has 2 as auxiliary argument.
   */
  std::vector<int> _arguments;
  
  /**
   * Default constructor.
   * It creates a new instance of a null constraint with a
   * new unique id.
   * It sets all the other members to null.
   */
  Constraint ();
  
  /**
   * Create a shared pointer from this instance.
   * @return a shared pointer to Constraint object.
   */
  virtual ConstraintPtr get_this_shared_ptr ();
  
public:
  virtual ~Constraint ();
  
  //! Get unique (global) id of this constraint.
  size_t get_unique_id () const;
  
  /**
   * Get number id of this constraint.
   * @note same type of constraints have same number_id.
   */
  int get_number_id () const;
  
  //! Get the name id of this constraint.
  std::string get_name () const;
  
  //! Get the weight of this constraint.
  int get_weight () const;
  
  /**
   * Set the consistency level for this
   * constraints. Different consistency
   * levels are implemented with different algorithms
   * and may require different computational times.
   */
  void set_consistency_level ( ConsistencyType con_type );
  
  /**
   * Increse current weight.
   * @param weight the weight to add to the current weight
   *        (default: 1).
   */
  void increase_weight ( int weight = 1 );
  
  /**
   * Decrease current weight.
   * @param weight the weight to decrease from the current weight
   *        (default: 1).
   */
  void decrease_weight ( int weight = 1 );
  
  /**
   * Get the size of the scope of this constraint,
   * i.e., the number of FD variables which is defined on.
   * @note The size of the scope does not correspond to the formal
   *       definition of the constraint but with the actual number
   *       of variables within the scope of a given constraint.
   *       For example:
   *          int_eq ( x, y ) has _scope_size equal to 2;
   *          int_eq ( x, 1 ) has _scope_size equal to 1.
   */
  size_t get_scope_size () const;
  
  //!Get the size of the auxiliary arguments of this constraint.
  size_t get_arguments_size () const;
  
  /**
   * Set an event as triggering event for re-evaluation of this constraint.
   * @param event the event that will trigger the re-evaluation of this constriant.
   * @note default: CHANGE_EVT.
   * @note different constraints should specilize this method
   *       with the appropriate list of events.
   */
  virtual void set_event ( EventType event = EventType::CHANGE_EVT );
  
  /**
   * It returns the list of events that trigger
   * a given constraint.
   */
  const std::vector<EventType>& events () const;
  
  /**
   * It returns the list of auxiliary arguments 
   * of a given constraint.
   */
  const std::vector<int>& arguments () const;
  
  /**
   * It receives an update about an action that has been
   * performed on some variables and it acts accordingly.
   * This method is used to trigger some actions when
   * this observer observes a change in the state of some
   * observed subject.
   * @param e an object of type Event that specifies the event
   *        that triggered the update.
   */
  virtual void update ( EventType e );
  
  /**
   * It returns a vector of (pointers to) constraints which are used
   * to decompose this constraint. It actually creates a
   * decomposition (possibly also creating variables), but it
   * does not impose the constraints.
   * @return a vector of (pointers to) constraints used to 
   *         decompose this constraint.
   */
  virtual std::vector<ConstraintPtr> decompose () const;
  
  /**
   * It returns the vector of (pointers to) variables that
   * correspond to the variables for which the domains have 
   * been modified by the propagation/consistency of this
   * constraint w.r.t. a given event.
   * @param event the event to that may be happened on some 
   *        domain of the variables of the scope of this constraint.
   * @return a vector of (pointers to) variables which 
   *         domains have been modified after the propagation
   *         of this constraint. It returns null if no
   *         domain has been modified.
   */
  virtual std::vector<VariablePtr> changed_vars_from_event ( EventType event ) const;
  
  /**
   * It returns the vector of (pointers to) all variables
   * for which the corresponding domains have been modified 
   * by the propagation/consistency of this constraint.
   * @return a vector of (pointers to) variables which
   *         domains have been modified after the propagation
   *         of this constraint. It returns null if no
   *         domain has been modified.
   */
  virtual std::vector<VariablePtr> changed_vars () const;
  
  /**
   * It checks if the constraint has reached the fixed point,
   * i.e., it checks whether no events happened on the domains
   * of the variables in the scope of the this constraint.
   */
  virtual bool fix_point () const;
  
  /**
   * It returns an integer value that can be used
   * to represent how much the current constraint is
   * unsatisfied. This function can be used to
   * implement some heuristics for optimization problems.
   * @return an integer value representing how much this 
   *         constraint is unsatisfied. It returns 0 if
   *         this constraint is satisfied.
   */
  virtual int unsat_level () const;
  
  /**
   * It returns the vector of (shared) pointers
   * of all the variables involved in a
   * given constraint (i.e., its scope).
   */
  virtual const std::vector<VariablePtr> scope () const = 0;
  
  /**
   * It attaches this constraint (observer) to the list of
   * the variables in its scope. 
   * When a variable changes state, this constraint could be
   * automatically notified (depending on the variable).
   */
  virtual void attach_me_to_vars () = 0;
  
  /**
   * It is a (most probably incomplete) consistency function which
   * removes the values from variable domains. Only values which
   * do not have any support in a solution space are removed.
   */
  virtual void consistency () = 0;
  
  /**
   * It checks if the constraint is satisfied.
   * @return true if the constraint if for certain satisfied,
   *         false otherwise.
   * @note If this function is incorrectly implementd, 
   *       a constraint may not be satisfied in a solution.
   */
  virtual bool satisfied () = 0;
  
  /**
   * It removes the constraint by removing this constraint
   * from all variables in its scope.
   */
  virtual void remove_constraint () = 0;
  
  //! Prints info.
  virtual void print () const = 0;
  
  //! Prints the semantic of this constraint.
  virtual void print_semantic () const = 0;
};


#endif
