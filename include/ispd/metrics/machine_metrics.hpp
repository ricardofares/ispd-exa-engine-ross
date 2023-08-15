#ifndef ISPD_METRICS_MACHINE_HPP
#define ISPD_METRICS_MACHINE_HPP

namespace ispd::metrics {

/// \struct MachineMetrics
///
/// \brief This structure encapsulates various metrics and information related
///        to a machine's performance and behavior during simulation.
///
/// The `MachineMetrics` structure collects and organizes metrics that enable
/// users to analyze and evaluate the performance of machines within a
/// simulated environment.
struct MachineMetrics final {
  /// \name Processing-related Metrics
  double m_ProcMFlops; ///< The total amount of megaflops processed by this
                       ///< machine.

  /// \brief The total time in seconds that the machine spent processing tasks.
  double m_ProcTime; ///< The total time in seconds that the machine spent
                     ///< processing tasks.

  double m_ProcWaitingTime; ///< The waiting time in seconds that tasks spent in
                            ///< the processing queue before being executed by
                            ///< the machine.

  /// \name Tasks-related Metrics
  unsigned m_ProcTasks; ///< The total nubmer of tasks successfully processed by
                        ///< the machine.

  unsigned
      m_ForwardedTasks; ///< The total number of tasks forwarded by the machine
                        ///< to other components within the simulation.

  /// \name Energy-related Metrics
  double
      m_PowerIdle; ///< The power consumption (in W) when the machine is idle.

  double
      m_PowerMax; ///< The power consumptio (in W) that the machine can reach.
};

} // namespace ispd::metrics

#endif // ISPD_METRICS_MACHINE_HPP
