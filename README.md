*This project has been created as part of the 42 curriculum by ecakiray.*

# Codexion

## Description

Codexion is a multithreaded simulation in which coders compete for shared USB dongles. Each coder is represented by a thread and needs two adjacent dongles before compiling.

Each dongle owns a priority queue implemented as a binary heap. Requests are ordered using one of two scheduling policies:

- `fifo`: the earliest request is served first.
- `edf`: the request with the earliest burnout deadline is served first. Equal deadlines are ordered by request sequence.

A coder can acquire the dongles only when its request is at the top of both queues, both dongles are free, and their cooldown periods have ended.

A separate monitor thread detects burnout and stops the simulation when a coder burns out or when every coder completes the required number of compilations.

## Instructions

### Compilation

```sh
cd coders
make
```

The executable is named `codexion`.

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile \
time_to_debug time_to_refactor number_of_compiles_required \
dongle_cooldown scheduler
```

The scheduler must be either `fifo` or `edf`.

Examples:

```sh
./codexion 5 1000 100 100 100 3 60 fifo
./codexion 5 1000 100 100 100 3 60 edf
```

### Makefile rules

```sh
make
make clean
make fclean
make re
```

## Resources

The following resources were used during development:

- The Codexion subject.
- Previously evaluated Philosophers projects, which were useful as initial references for thread organization, mutex protection, monitoring, timing, and resource cleanup.
- POSIX manual pages for `pthread_create`, `pthread_join`, mutexes, and condition variables.
- Manual pages for `gettimeofday` and `usleep`.
- 42 Norminette documentation.
- Documentation and examples of binary heap implementations.

The previously evaluated Philosophers projects were used to understand common multithreading patterns. Codexion-specific features, including FIFO/EDF scheduling, request heaps, dongle cooldown, and dual-queue arbitration, were developed specifically for this project.

### AI usage

AI tools were used as a learning and review aid to explain concurrency concepts, review the project structure, identify Norminette issues, discuss possible race conditions, propose test cases and generating this README file.

Suggested changes were applied incrementally, reviewed, compiled, and tested by the author. The synchronization design and program behavior were also checked manually.

## Blocking cases handled

### Deadlock prevention

When two dongles must be locked, their mutexes are always locked in ascending dongle ID order. This prevents circular waiting between coder threads.

### Fair scheduling

Every request is inserted into the queues of both required dongles. A coder can acquire its dongles only when the same request is at the top of both queues.

FIFO uses a global request sequence. EDF uses the coder's burnout deadline and the sequence number as a deterministic tie-breaker.

### Dongle cooldown

After a dongle is released, its next available timestamp is recorded. Waiting threads use a timed condition wait until the cooldown expires or another relevant event occurs.

### Single coder

When there is only one coder, there is only one dongle. The coder cannot acquire the same dongle twice and eventually burns out as expected.

### Synchronized start

Coder threads and the monitor wait on a common start condition. Their timing values are initialized before the simulation begins, preventing early threads from receiving an unfair advantage.

### Simulation termination

The simulation stops when a coder burns out or when every coder reaches the required number of compilations. Waiting threads are awakened so that they can exit cleanly.

Completed coders do not create additional dongle requests.

### Serialized logging

Log output is protected by a mutex. This prevents messages from different threads from being printed on the same line or after the simulation has stopped.

## Thread synchronization mechanisms

### Dongle mutexes

Each dongle has its own `pthread_mutex_t`. The mutex protects:

- The current dongle owner.
- The dongle request heap.
- The cooldown timestamp.

### State mutex

The state mutex protects shared simulation data, including:

- The stop flag.
- Compile counters.
- Last compile timestamps.
- Request sequence numbers.

### Log mutex

The log mutex serializes terminal output and prevents messages from interleaving.

### Wait mutex and condition variable

The wait mutex is associated with a shared `pthread_cond_t`. It is used to:

- Wait for the simulation to start.
- Wake coders when dongles are released.
- Wait for dongle cooldown periods.
- Wake all waiting threads when the simulation stops.

Condition variables avoid continuous busy waiting while coders are waiting for resources.

## Evaluation checklist

Run the following commands from the `coders` directory.

### Norm and compilation

```sh
norminette include src

make fclean
make all
make
make clean
make all
make re
```

The second consecutive `make` should report that there is nothing to build.

### Basic functional tests

```sh
./codexion 5 1000 100 100 100 3 60 fifo
./codexion 5 1000 100 100 100 3 60 edf
```

Expected behavior:

- No interleaved log lines.
- No duplicated dongle ownership.
- Every coder completes the required number of compilations.
- The program exits without hanging.

### Single-coder burnout

```sh
./codexion 1 300 100 100 100 2 60 fifo
```

A single coder has only one dongle and must eventually burn out.

### Forced burnout

```sh
./codexion 2 150 100 100 100 3 60 fifo
```

The simulation must print one burnout message and stop cleanly.

### Dongle cooldown

```sh
./codexion 2 1000 100 60 60 2 200 fifo
```

After release, the dongles must not be acquired again before the cooldown expires.

### Stress test

```sh
timeout 60s ./codexion 200 100000 60 60 60 2 60 fifo
timeout 60s ./codexion 200 100000 60 60 60 2 60 edf
```

The program must finish without crashes, deadlocks or corrupted output.

### Invalid arguments

Each command must print an error and return a non-zero exit status.

```sh
./codexion
./codexion 0 1000 100 100 100 2 60 fifo
./codexion -2 1000 100 100 100 2 60 fifo
./codexion abc 1000 100 100 100 2 60 fifo
./codexion 2 1000 100 100 100 2 60 random
./codexion 2147483648 1000 100 100 100 2 60 fifo
```

The exit status can be checked immediately after a command:

```sh
echo "Exit code: $?"
```

### Memory leaks

```sh
timeout 60s valgrind \
	--leak-check=full \
	--show-leak-kinds=all \
	--errors-for-leak-kinds=all \
	--error-exitcode=42 \
	./codexion 5 100000 60 60 60 2 60 fifo
```

Expected result:

```text
All heap blocks were freed
ERROR SUMMARY: 0 errors
```

### Helgrind

```sh
timeout 60s valgrind \
	--tool=helgrind \
	--fair-sched=yes \
	--error-exitcode=42 \
	./codexion 5 100000 60 60 60 2 60 fifo
```

### DRD

```sh
timeout 60s valgrind \
	--tool=drd \
	--error-exitcode=42 \
	./codexion 5 100000 60 60 60 2 60 fifo
```

For the commands using `timeout` and `--error-exitcode=42`:

- Exit code `0`: the test completed successfully.
- Exit code `42`: Valgrind reported an error.
- Exit code `124`: the command reached the timeout.
