import matplotlib.pyplot as plt


# =========================================================
# SCHEDULER COMPARISON
# =========================================================

# Results obtained from the three schedulers

schedulers = ["FIFO", "RR", "MLFQ"]

response_time = [22, 1.5, 1.5]
turnaround_time = [41, 7.25, 6.25]
waiting_time = [22, 5, 4]


print("========================================")
print("       SCHEDULER COMPARISON")
print("========================================")
print()

print("Scheduler   Response   Turnaround   Waiting")
print("--------------------------------------------")

for i in range(3):
    print("%-10s %-10.2f %-12.2f %.2f" %
          (schedulers[i],
           response_time[i],
           turnaround_time[i],
           waiting_time[i]))

print()
print("========================================")
print()


# Create comparison bar chart

x = range(len(schedulers))
width = 0.25

plt.figure(figsize=(10, 6))

plt.bar(
    [i - width for i in x],
    response_time,
    width,
    label="Response Time"
)

plt.bar(
    x,
    turnaround_time,
    width,
    label="Turnaround Time"
)

plt.bar(
    [i + width for i in x],
    waiting_time,
    width,
    label="Waiting Time"
)

plt.xlabel("Scheduler")
plt.ylabel("Average Time (ticks)")
plt.title("FIFO vs RR vs MLFQ Scheduling Performance")

plt.xticks(x, schedulers)
plt.legend()


# Watermark
plt.text(
    0.5, 0.5, "shreshth.mehrotra",
    ha="center",
    va="center",
    transform=plt.gca().transAxes,
    fontsize=12,
    color="gray",
    alpha=0.7
)


plt.tight_layout()

plt.savefig(
    "scheduler_comparison.png",
    dpi=300
)

plt.close()


# =========================================================
# MLFQ SCHEDULER TIMELINE
# =========================================================
#
# Data extracted from the xv6 schedulertest output.
#
# Test processes start at absolute tick 46.
# Therefore:
#
#     elapsed tick = absolute tick - 46
#
# Priority boosts:
#     absolute tick 48 -> elapsed tick 2
#     absolute tick 96 -> elapsed tick 50
#
# Queue numbering:
#     Q0 = highest priority
#     Q3 = lowest priority
# =========================================================


START_TICK = 46


# ---------------------------------------------------------
# Process information
#
# Each transition is:
#     (absolute_tick, new_queue)
#
# Every process initially enters Q0.
# ---------------------------------------------------------

processes = {

    4: {
        "start": 46,
        "end": 99,
        "transitions": [
            (47, 1),
            (48, 0),   # Priority boost
            (51, 1),
            (67, 2),
            (96, 0),   # Priority boost
            (96, 1),   # Immediately exhausts Q0 quantum
        ]
    },

    5: {
        "start": 46,
        "end": 102,
        "transitions": [
            (48, 1),
            (55, 2),
            (75, 3),
            (96, 0),   # Priority boost
            (97, 1),
        ]
    },

    6: {
        "start": 46,
        "end": 114,
        "transitions": [
            (49, 1),
            (59, 2),
            (83, 3),
            (96, 0),   # Priority boost
            (98, 1),
            (106, 2),
        ]
    },

    7: {
        "start": 46,
        "end": 123,
        "transitions": [
            (50, 1),
            (63, 2),
            (91, 3),
            (96, 0),   # Priority boost
            (99, 1),
            (110, 2),
            (122, 3),
        ]
    }
}


# ---------------------------------------------------------
# Construct queue timeline for every process
# ---------------------------------------------------------

timeline = {}

for pid, info in processes.items():

    current_queue = 0
    transition_index = 0

    points = []

    for tick in range(info["start"], info["end"] + 1):

        # Apply all queue transitions occurring at this tick.
        while (
            transition_index < len(info["transitions"])
            and info["transitions"][transition_index][0] == tick
        ):
            current_queue = info["transitions"][transition_index][1]
            transition_index += 1

        elapsed_tick = tick - START_TICK

        points.append((elapsed_tick, current_queue))

    timeline[pid] = points


# =========================================================
# CREATE SCATTER PLOT
# =========================================================

plt.figure(figsize=(14, 8))


# ---------------------------------------------------------
# Plot each process
# ---------------------------------------------------------

for pid, points in timeline.items():

    x = [point[0] for point in points]
    y = [point[1] for point in points]

    plt.scatter(
        x,
        y,
        s=28,
        label=f"PID {pid}"
    )


# =========================================================
# PRIORITY BOOSTS
# =========================================================

boost_ticks = [48, 96]

for tick in boost_ticks:

    elapsed = tick - START_TICK

    # Vertical line showing the boost instant
    plt.axvline(
        x=elapsed,
        linestyle="--",
        linewidth=1.5
    )

    # Explicit annotation
    plt.annotate(
        f"Priority Boost\n(tick {tick})",
        xy=(elapsed, 0),
        xytext=(elapsed + 1.5, 3.05),
        arrowprops=dict(
            arrowstyle="->",
            linewidth=1.2
        ),
        fontsize=10,
        ha="left",
        va="center"
    )


# =========================================================
# AXES AND LABELS
# =========================================================

plt.yticks(
    [0, 1, 2, 3],
    [
        "Q0 (Highest)",
        "Q1",
        "Q2",
        "Q3 (Lowest)"
    ]
)

plt.xlabel(
    "Elapsed ticks since start of schedulertest (tick 46)",
    fontsize=11
)

plt.ylabel(
    "MLFQ Queue",
    fontsize=11
)

plt.title(
    "MLFQ Scheduling Timeline: Queue Movement and Priority Boosts",
    fontsize=14
)


# ---------------------------------------------------------
# Grid
# ---------------------------------------------------------

plt.grid(
    True,
    alpha=0.25
)


# ---------------------------------------------------------
# Make queue levels easier to distinguish
# ---------------------------------------------------------

plt.ylim(-0.3, 3.4)
plt.xlim(0, 78)


# ---------------------------------------------------------
# Legend
# ---------------------------------------------------------

plt.legend(
    loc="upper right",
    ncol=2,
    fontsize=10
)


# ---------------------------------------------------------
# Watermark
# ---------------------------------------------------------

plt.text(
    0.5, 0.5, "shreshth.mehrotra",
    ha="center",
    va="center",
    transform=plt.gca().transAxes,
    fontsize=12,
    color="gray",
    alpha=0.7
)


# ---------------------------------------------------------
# Layout
# ---------------------------------------------------------

plt.tight_layout()


# =========================================================
# SAVE GRAPH
# =========================================================

plt.savefig(
    "mlfq_timeline.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()