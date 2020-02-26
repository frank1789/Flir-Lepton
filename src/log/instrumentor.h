//
// Basic instrumentation profiler by Cherno

// Usage: include this header file somewhere in your code (eg. precompiled
// header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");        // Begin session
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like
//     this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();                        // End Session
//
// You will probably want to macro-fy this, to switch on/off easily and use
// things like __FUNCSIG__ for the profile name.

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <string>
#include <thread>

#define PROFILING 1
#if PROFILING
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || \
    (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define CLIENT_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define CLIENT_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define CLIENT_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || \
    (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define CLIENT_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define CLIENT_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define CLIENT_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define CLIENT_FUNC_SIG __func__
#else
#define CLIENT_FUNC_SIG "FUNC_SIG unknown!"
#endif
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(CLIENT_FUNC_SIG)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

struct ProfileResult {
  std::string Name;
  long long Start, End;
  uint32_t ThreadID;
};

struct InstrumentationSession {
  std::string Name;
};

class Instrumentor {
public:
  Instrumentor() : m_CurrentSession(nullptr), m_ProfileCount(0) {}

  void BeginSession(const std::string &name,
                    const std::string &filepath = "results.json") {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_CurrentSession) {
      InternalEndSession();
    }
    m_OutputStream.open(filepath);
    if (m_OutputStream.is_open()) {
      m_CurrentSession = new InstrumentationSession{name};
      WriteHeader();
    }
  }

  void EndSession() {
    std::lock_guard<std::mutex> lock(m_mutex);
    InternalEndSession();
  }

  void WriteProfile(const ProfileResult &result) {
    if (m_ProfileCount++ > 0) m_OutputStream << ",";

    std::string name = result.Name;
    std::replace(name.begin(), name.end(), '"', '\'');

    m_OutputStream << "{";
    m_OutputStream << "\"cat\":\"function\",";
    m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
    m_OutputStream << "\"name\":\"" << name << "\",";
    m_OutputStream << "\"ph\":\"X\",";
    m_OutputStream << "\"pid\":0,";
    m_OutputStream << "\"tid\":" << result.ThreadID << ",";
    m_OutputStream << "\"ts\":" << result.Start;
    m_OutputStream << "}";

    m_OutputStream.flush();
  }

  void WriteHeader() {
    m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
    m_OutputStream.flush();
  }

  void WriteFooter() {
    m_OutputStream << "]}";
    m_OutputStream.flush();
  }

  static Instrumentor &Get() {
    static Instrumentor instance;
    return instance;
  }

 private:
  void InternalEndSession() {
    if (m_CurrentSession) {
      WriteFooter();
      m_OutputStream.close();
      delete m_CurrentSession;
      m_CurrentSession = nullptr;
    }
  }

  InstrumentationSession *m_CurrentSession;
  std::ofstream m_OutputStream;
  int m_ProfileCount;
  std::mutex m_mutex;
};

class InstrumentationTimer {
 public:
  explicit InstrumentationTimer(const char *name)
      : m_Name(name),
        m_StartTimepoint(std::chrono::high_resolution_clock::now()),
        m_Stopped(false) {}

  ~InstrumentationTimer() {
    if (!m_Stopped) Stop();
  }

  void Stop() {
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    long long start = std::chrono::time_point_cast<std::chrono::microseconds>(
                          m_StartTimepoint)
                          .time_since_epoch()
                          .count();
    long long end =
        std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
            .time_since_epoch()
            .count();

    uint32_t threadID =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

    m_Stopped = true;
  }

 private:
  const char *m_Name;
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
  bool m_Stopped;
};
