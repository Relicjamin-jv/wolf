#pragma once

#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <core/audio.hpp>
#include <core/input.hpp>
#include <core/virtual-display.hpp>
#include <cstddef>
#include <eventbus/event_bus.hpp>
#include <helpers/tsqueue.hpp>
#include <immer/array.hpp>
#include <immer/atom.hpp>
#include <immer/box.hpp>
#include <immer/map.hpp>
#include <immer/vector.hpp>
#include <moonlight/control.hpp>
#include <moonlight/data-structures.hpp>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <state/serialised_config.hpp>
#include <string_view>

namespace wolf::core::events {

struct PairSignal {
  const std::string event_type = "pair";
  std::string client_ip;
  std::string host_ip;
  std::shared_ptr<boost::promise<std::string>> user_pin;
};

struct PlugDeviceEvent {
  const std::string event_type = "plug_device";
  std::size_t session_id;
  std::vector<std::map<std::string, std::string>> udev_events;
  std::vector<std::pair<std::string, std::vector<std::string>>> udev_hw_db_entries;
};

struct UnplugDeviceEvent {
  const std::string event_type = "unplug_device";
  std::size_t session_id;
  std::vector<std::map<std::string, std::string>> udev_events;
  std::vector<std::pair<std::string, std::vector<std::string>>> udev_hw_db_entries;
};

using devices_atom_queue = TSQueue<immer::box<events::PlugDeviceEvent>>;

struct Runner {

  virtual void run(std::size_t session_id,
                   std::string_view app_state_folder,
                   std::shared_ptr<devices_atom_queue> plugged_devices_queue,
                   const immer::array<std::string> &virtual_inputs,
                   const immer::array<std::pair<std::string, std::string>> &paths,
                   const immer::map<std::string, std::string> &env_variables,
                   std::string_view render_node) = 0;

  virtual rfl::TaggedUnion<"type", wolf::config::AppCMD, wolf::config::AppDocker> serialize() = 0;
};

struct App {
  moonlight::App base;

  std::string h264_gst_pipeline;
  std::string hevc_gst_pipeline;
  std::string av1_gst_pipeline;

  std::string render_node;

  std::string opus_gst_pipeline;
  bool start_virtual_compositor;
  std::shared_ptr<Runner> runner;
  moonlight::control::pkts::CONTROLLER_TYPE joypad_type;
};

using MouseTypes = std::variant<input::Mouse, virtual_display::WaylandMouse>;
using KeyboardTypes = std::variant<input::Keyboard, virtual_display::WaylandKeyboard>;
using JoypadTypes = std::variant<input::XboxOneJoypad, input::SwitchJoypad, input::PS5Joypad>;
using JoypadList = immer::map<int /* controller number */, std::shared_ptr<JoypadTypes>>;

enum class ColorRange {
  JPEG,
  MPEG
};

enum class ColorSpace : int {
  BT601,
  BT709,
  BT2020
};

/**
 * A VideoSession is created after the param exchange over RTSP
 */
struct VideoSession {
  const std::string event_type = "video_session";

  wolf::core::virtual_display::DisplayMode display_mode;
  std::string gst_pipeline;

  // A unique ID that identifies this session
  std::size_t session_id;

  std::uint16_t port;
  int timeout_ms;

  int packet_size;
  int frames_with_invalid_ref_threshold;
  int fec_percentage;
  int min_required_fec_packets;
  long bitrate_kbps;
  int slices_per_frame;

  ColorRange color_range;
  ColorSpace color_space;

  std::string client_ip;
};

struct AudioSession {
  const std::string event_type = "audio_session";

  std::string gst_pipeline;

  // A unique ID that identifies this session
  std::size_t session_id;

  bool encrypt_audio;
  std::string aes_key;
  std::string aes_iv;

  std::uint16_t port;
  std::string client_ip;

  int packet_duration;
  wolf::core::audio::AudioMode audio_mode;
};

struct IDRRequestEvent {
  const std::string event_type = "idr_request";

  // A unique ID that identifies this session
  std::size_t session_id;
};

struct PauseStreamEvent {
  const std::string event_type = "pause_stream";
  std::size_t session_id;
};

struct ResumeStreamEvent {
  const std::string event_type = "resume_stream";
  std::size_t session_id;
};

struct StopStreamEvent {
  const std::string event_type = "stop_stream";
  std::size_t session_id;
};

struct RTPVideoPingEvent {
  const std::string event_type = "rtp_video_ping";
  std::string client_ip;
  unsigned short client_port;
};

struct RTPAudioPingEvent {
  const std::string event_type = "rtp_audio_ping";
  std::string client_ip;
  unsigned short client_port;
};

struct StreamSession;

using EventBusHandlers = dp::handler_registration<immer::box<PlugDeviceEvent>,
                                                  immer::box<PairSignal>,
                                                  immer::box<UnplugDeviceEvent>,
                                                  immer::box<StreamSession>,
                                                  immer::box<VideoSession>,
                                                  immer::box<AudioSession>,
                                                  immer::box<IDRRequestEvent>,
                                                  immer::box<PauseStreamEvent>,
                                                  immer::box<ResumeStreamEvent>,
                                                  immer::box<StopStreamEvent>,
                                                  immer::box<RTPVideoPingEvent>,
                                                  immer::box<RTPAudioPingEvent>>;
using EventBusType = dp::event_bus<immer::box<PlugDeviceEvent>,
                                   immer::box<PairSignal>,
                                   immer::box<UnplugDeviceEvent>,
                                   immer::box<StreamSession>,
                                   immer::box<VideoSession>,
                                   immer::box<AudioSession>,
                                   immer::box<IDRRequestEvent>,
                                   immer::box<PauseStreamEvent>,
                                   immer::box<ResumeStreamEvent>,
                                   immer::box<StopStreamEvent>,
                                   immer::box<RTPVideoPingEvent>,
                                   immer::box<RTPAudioPingEvent>>;
using EventsVariant = std::variant<immer::box<PlugDeviceEvent>,
                                   immer::box<PairSignal>,
                                   immer::box<UnplugDeviceEvent>,
                                   immer::box<StreamSession>,
                                   immer::box<VideoSession>,
                                   immer::box<AudioSession>,
                                   immer::box<IDRRequestEvent>,
                                   immer::box<PauseStreamEvent>,
                                   immer::box<ResumeStreamEvent>,
                                   immer::box<StopStreamEvent>,
                                   immer::box<RTPVideoPingEvent>,
                                   immer::box<RTPAudioPingEvent>>;

/**
 * A StreamSession is created when a Moonlight user call `launch`
 *
 * This will then be fired up in the event_bus so that the rtsp, command, audio and video threads
 * can start working their magic.
 */
struct StreamSession {
  const std::string event_type = "stream_session";
  moonlight::DisplayMode display_mode;
  int audio_channel_count;

  std::shared_ptr<EventBusType> event_bus;
  std::shared_ptr<App> app;
  std::string app_state_folder;

  // gcm encryption keys
  std::string aes_key;
  std::string aes_iv;

  // client info
  std::size_t session_id;
  std::string ip;

  unsigned short video_stream_port;
  unsigned short audio_stream_port;

  /**
   * Optional: the wayland display for the current session.
   * Will be only set during an active streaming and destroyed on stream end.
   */
  std::shared_ptr<immer::atom<virtual_display::wl_state_ptr>> wayland_display =
      std::make_shared<immer::atom<virtual_display::wl_state_ptr>>();

  // virtual devices
  std::shared_ptr<std::optional<MouseTypes>> mouse = std::make_shared<std::optional<MouseTypes>>();
  std::shared_ptr<std::optional<KeyboardTypes>> keyboard = std::make_shared<std::optional<KeyboardTypes>>();

  std::shared_ptr<immer::atom<JoypadList>> joypads = std::make_shared<immer::atom<JoypadList>>();

  std::shared_ptr<std::optional<input::PenTablet>> pen_tablet =
      std::make_shared<std::optional<input::PenTablet>>(); /* Optional, will be set on first use */
  std::shared_ptr<std::optional<input::TouchScreen>> touch_screen =
      std::make_shared<std::optional<input::TouchScreen>>(); /* Optional, will be set on first use */
};

} // namespace wolf::core::events