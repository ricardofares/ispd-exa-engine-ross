#ifndef ISPD_WORKLOAD_HPP
#define ISPD_WORKLOAD_HPP

#include <ross.h>
#include <memory>

namespace ispd::workload {

/// \class Workload
///
/// \brief A base class representing a workload to be generated for a
/// simulation.
///
class Workload {
protected:
  /// \brief The remaining tasks to be generated by this workload.
  ///
  /// The `m_RemainingTasks` member stores the count of tasks that are yet to be
  /// generated by the workload. It is decremented as tasks are generated during
  /// the simulation. This information allows simulation components to keep
  /// track of the workload progress and workload generation termination
  /// conditions.
  unsigned m_RemainingTasks;

public:
  /// \brief Constructor for the Workload class.
  ///
  /// Creates a new `Workload` object with the specified number of remaining
  /// tasks to be generated.
  ///
  /// \param remainingTasks The total number of tasks that need to be generated
  ///                       by the workload.
  explicit Workload(const unsigned remainingTasks)
      : m_RemainingTasks(remainingTasks) {}

  /// \brief Generate the workload, setting the processing and communication
  ///        sizes depending on the generation policy.
  ///
  /// This pure virtual function defines the interface for workload generation
  /// in derived classes. Each derived workload class must implement this
  /// function to generate the workload's processing and communication sizes
  /// based on the specific generation policy. The actual logic for generating
  /// workload characteristics can vary between different workload types.
  ///
  /// \param rng The logical process reversible-pseudonumber random generator.
  /// \param procSize A reference to the processing size to be updated based on
  ///                 the generated workload.
  /// \param commSize A reference to the communication size to be updated based
  ///                 on the generated workload.
  virtual void generateWorkload(tw_rng_stream *rng, double &procSize,
                                double &commSize) = 0;

  /// \brief Reverse the workload generation, necessary due to the Time Warp's
  ///        rollback mechanism.
  ///
  /// This pure virtual function defines the interface for reversing the
  /// workload generation in derived classes. Time Warp is a distributed
  /// discrete-event simulation technique that allows for virtual time rollback
  /// to correct causality errors. To support rollback, this function enables
  /// the workload generation to be reversed, so the simulation can roll back
  /// the workload's state if necessary.
  ///
  /// \param rng The logical process reversible-pseudorandom number generator.
  virtual void reverseGenerateWorkload(tw_rng_stream *rng) = 0;

  /// \brief Get the remaining tasks to be generated by this workload.
  ///
  /// \returns The number of tasks that are yet to be generated by the workload.
  ///
  /// \note This function provides access to the `m_RemainingTasks` member,
  ///       which stores the count of remaining tasks. It is used by simulation
  ///       components to monitor the progress of workload generation and
  ///       determine workload generation termination conditions.
  inline unsigned getRemainingTasks() { return m_RemainingTasks; }
};

/// \class ConstantWorkload
///
/// \brief A derived class representing a constant workload for simulation
///        tasks, all generated tasks will have the same workload.
///
class ConstantWorkload : public Workload {
  /// \brief The constant processing size to be set for all generated tasks.
  ///
  /// The `m_ConstantProcSize` member stores the constant processing size that
  /// will be assigned to all tasks generated by this workload. This value
  /// remains constant throughout the simulation for all tasks generated using
  /// this workload.
  double m_ConstantProcSize;

  /// \brief The constant communication size to be set for all generated tasks.
  ///
  /// The `m_ConstantCommSize` member stores the constant communication size
  /// that will be assigned to all tasks generated by this workload. This value
  /// remains constant throughout the simulation for all tasks generated using
  /// this workload.
  double m_ConstantCommSize;

public:
  /// \brief ConstantWorkload class constructor.
  ///
  /// Creates a new `ConstantWorkload` object with the specified parameters.
  ///
  /// \param remainingTasks The total number of tasks that need to be generated
  ///                       by the workload.
  /// \param constantProcSize The constant processing size to be set for all
  ///                         generated tasks.
  /// \param constantCommSize The constant communication size to be set for
  ///                         all generated tasks.
  ///
  /// \note The constructor calls the base class constructor
  /// `Workload(remainingTasks)` to initialize the workload with the number of
  /// remaining tasks. It also performs sanity checks to ensure that both
  /// `constantProcSize` and `constantCommSize` are positive values. If either
  /// value is not positive, the constructor sends an error message and aborts
  /// the program. The constructor prints debug information about the constant
  /// processing and communication sizes, as well as the number of remaining
  /// tasks.
  explicit ConstantWorkload(const unsigned remainingTasks,
                            const double constantProcSize,
                            const double constantCommSize)
      : Workload(remainingTasks), m_ConstantProcSize(constantProcSize),
        m_ConstantCommSize(constantCommSize) {
    if (constantProcSize <= 0.0)
      ispd_error("Constant processing size must be positive (Specified "
                 "constant processing size: %lf).",
                 constantProcSize);

    if (constantCommSize <= 0.0)
      ispd_error("Constant communication size must be positive (Specified "
                 "constant communication size: %lf).",
                 constantCommSize);

    ispd_debug("[Constant Workload] PS: %lf, CS: %lf, RT: %u.",
               constantProcSize, constantCommSize, remainingTasks);
  }

  /// \brief Generate the constant workload, setting the processing and
  /// communication sizes.
  ///
  /// This member function implements the workload generation logic for the
  /// `ConstantWorkload` class. It sets the processing size (`procSize`) and
  /// communication size (`commSize`) to the constant values specified during
  /// object construction. The remaining tasks counter in the base class
  /// (`Workload::m_RemainingTasks`) is decremented after generating the
  /// workload for a task.
  ///
  /// \param rng The logical process reversible-pseudorandom number generator.
  /// \param procSize A reference to the processing size to be updated based on
  ///                 the constant workload generation.
  /// \param commSize A reference to the communication size to be updated based
  ///                 on the constant workload generation.
  void generateWorkload(tw_rng_stream *rng, double &procSize,
                        double &commSize) override {
    procSize = m_ConstantProcSize;
    commSize = m_ConstantCommSize;

    Workload::m_RemainingTasks--;
  }

  /// \brief Reverse the constant workload generation, needed due to Time Warp's
  /// rollback mechanism.
  ///
  /// This member function reverses the workload generation for the
  /// `ConstantWorkload` class to support Time Warp's rollback mechanism. When a
  /// rollback occurs, the remaining tasks counter in the base class
  /// (`Workload::m_RemainingTasks`) is incremented to revert the effects of
  /// generating workload for a task.
  ///
  /// \param rng The logical process reversible-pseudorandom number generator.
  void reverseGenerateWorkload(tw_rng_stream *rng) override {
    Workload::m_RemainingTasks++;
  }
};

/// \class UniformWorkload
///
/// \brief A derived class representing a uniform workload for simulation tasks.
///
class UniformWorkload : public Workload {
  /// \brief The minimum processing size for generated tasks.
  double m_MinProcSize;

  /// \brief The maximum processing size for generated tasks.
  double m_MaxProcSize;

  /// \brief The minimum communication size for generated tasks.
  double m_MinCommSize;

  /// \brief The maximum communication size for generated tasks.
  double m_MaxCommSize;

public:
  /// \brief UniformWorkload class constructor.
  ///
  /// Creates a new `UniformWorkload` object with the specified parameters.
  ///
  /// \param remainingTasks The total number of tasks that need to be generated
  ///                       by the workload.
  /// \param minProcSize The minimum processing size to be set for generated
  ///                    tasks.
  /// \param maxProcSize The maximum processing size to be set for generated
  ///                    tasks.
  /// \param minCommSize The minimum communication size to be set for generated
  ///                    tasks.
  /// \param maxCommSize The maximum communication size to be set for generated
  ///                    tasks.
  ///
  explicit UniformWorkload(const unsigned remainingTasks,
                           const double minProcSize, const double maxProcSize,
                           const double minCommSize, const double maxCommSize)
      : Workload(remainingTasks), m_MinProcSize(minProcSize),
        m_MaxProcSize(maxProcSize), m_MinCommSize(minCommSize),
        m_MaxCommSize(maxCommSize) {
    if (minProcSize <= 0.0)
      ispd_error("Minimum processing size must be positive (Specified "
                 "minimum processing size: %lf).",
                 minProcSize);

    if (maxProcSize <= 0.0)
      ispd_error("Maximum processing size must be positive (Specified "
                 "maximum processing size: %lf).",
                 maxProcSize);

    if (minCommSize <= 0.0)
      ispd_error("Minimum communication size must be positive (Specified "
                 "minimum communication size: %lf).",
                 minCommSize);

    if (maxCommSize <= 0.0)
      ispd_error("Maximum communication size must be positive (Specified "
                 "maximum communication size: %lf).",
                 maxCommSize);

    ispd_debug("[Uniform Workload] PI: [%lf, %lf], CI: [%lf, %lf], RT: %u.",
               minProcSize, maxProcSize, minCommSize, maxCommSize,
               remainingTasks);
  }

  /// \brief Generate the uniform workload, setting the processing and
  /// communication sizes.
  ///
  /// This member function implements the workload generation logic for the
  /// `UniformWorkload` class. It sets the processing size (`procSize`) and
  /// communication size (`commSize`) to random values within the specified
  /// range of minimum and maximum processing and communication sizes. The
  /// remaining tasks counter in the base class (`Workload::m_RemainingTasks`)
  /// is decremented after generating the workload for a task.
  ///
  /// \param rng The logical process reversible-pseudorandom number generator.
  /// \param procSize A reference to the processing size to be updated based on
  /// the uniform workload generation. \param commSize A reference to the
  /// communication size to be updated based on the uniform workload generation.
  void generateWorkload(tw_rng_stream *rng, double &procSize,
                        double &commSize) override {
    // Generate random values for processing and communication sizes within the
    // specified range.
    procSize =
        m_MinProcSize + tw_rand_unif(rng) * (m_MaxProcSize - m_MinProcSize);
    commSize =
        m_MinCommSize + tw_rand_unif(rng) * (m_MaxCommSize - m_MinCommSize);

    // Update the workload information.
    Workload::m_RemainingTasks--;

    // Print a debug message informing the just generated task information and
    // the remaining tasks to be generated by this workload.
    ispd_debug("[Uniform Workload] Workload (%lf, %lf) generated. RT: %u.",
               procSize, commSize, Workload::m_RemainingTasks);
  }

  /// \brief Reverse the uniform workload generation, needed due to Time Warp's
  /// rollback mechanism.
  ///
  /// This member function reverses the workload generation for the
  /// `UniformWorkload` class to support Time Warp's rollback mechanism. When a
  /// rollback occurs, the remaining tasks counter in the base class
  /// (`Workload::m_RemainingTasks`) is incremented to revert the effects of
  /// generating workload for a task. Additionally, the two previous calls to
  /// `tw_rand_unif` that were used to generate the processing and communication
  /// sizes are reversed using `tw_rand_reverse_unif`.
  ///
  /// \param rng The logical process reversible-pseudorandom number generator.
  void reverseGenerateWorkload(tw_rng_stream *rng) override {
    /// The following two function calls reverse the use of two times
    /// of the `tw_rand_unif` at the `generateWorkload` member-function.
    ///
    ///   1. The first call reverse the pseudorandom number generation for
    ///      the processing size generation.
    ///
    ///   2. The second call reverse the pseudorandom nubmer genration for
    ///      the communication size generation.
    tw_rand_reverse_unif(rng);
    tw_rand_reverse_unif(rng);

    Workload::m_RemainingTasks++;

    ispd_debug("[Uniform Workload] Reversed. RT: %u.",
               Workload::m_RemainingTasks);
  }
};

/// \brief Create a new ConstantWorkload object with specified parameters.
///
/// This function is a helper function that creates and returns a new instance
/// of the ConstantWorkload class. It simplifies the process of creating
/// ConstantWorkload objects by encapsulating the object instantiation and
/// returning a pointer to the newly created object.
///
/// \param remainingTasks The total number of tasks that need to be generated by
///                       the workload.
/// \param constantProcSize The constant processing size to be set for all
///                         generated tasks.
/// \param constantCommSize The constant communication size to be set for
///                         all generated tasks.
///
/// \returns A pointer to the newly created ConstantWorkload object.
///
/// \note This function uses dynamic memory allocation to create the
///       ConstantWorkload object on the heap. The caller is responsible for
///       managing the object's memory and must eventually delete the object
///       when it is no longer needed to avoid memory leaks.
static inline ConstantWorkload *constant(const unsigned remainingTasks,
                                         const double constantProcSize,
                                         const double constantCommSize) {
  return new ConstantWorkload(remainingTasks, constantProcSize,
                              constantCommSize);
}

/// \brief Create a new UniformWorkload object with specified parameters.
///
/// This function is a helper function that creates and returns a new instance
/// of the UniformWorkload class. It simplifies the process of creating
/// UniformWorkload objects by encapsulating the object instantiation and
/// returning a pointer to the newly created object.
///
/// \param remainingTasks The total number of tasks that need to be generated by
///                       the workload.
/// \param minProcSize The minimum processing size to be set for generated
///                    tasks.
/// \param maxProcSize The maximum processing size to be set for generated
///                    tasks.
/// \param minCommSize The minimum communication size to be set for generated
///                    tasks.
/// \param maxCommSize The maximum communication size to be set for generated
///                    tasks.
///
/// \returns A pointer to the newly created UniformWorkload object.
///
/// \note This function uses dynamic memory allocation to create the
/// UniformWorkload object on the heap. The caller is responsible for managing
/// the object's memory and must eventually delete the object when it is no
/// longer needed to avoid memory leaks.
static inline UniformWorkload *uniform(const unsigned remainingTasks,
                                       const double minProcSize,
                                       const double maxProcSize,
                                       const double minCommSize,
                                       const double maxCommSize) {
  return new UniformWorkload(remainingTasks, minProcSize, maxProcSize,
                             minCommSize, maxCommSize);
}

}; // namespace ispd::workload

#endif // ISPD_WORKLOAD_HPP
