#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Svc_Handler.h>

class Client : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {

public:
  typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> super;
  enum { MAX_ITERATIONS = 7 };
  Client() : super(), iterations_(0), bytes_sent_(-1), bytes_to_send_(-1) {}

  int open(const ACE_INET_Addr &addr) {
    ACE_SOCK_Connector connector;
    if (connector.connect(this->peer(), addr) == -1) {
      ACE_ERROR_RETURN((LM_ERROR, "%p\n", "connect"), -1);
    }
    return 0;
  }

  int handle_output(ACE_HANDLE) {
    ACE_DEBUG((LM_INFO, "Client handle_output\n"));
    if (this->bytes_to_send_ < 0) {
      ACE_OS::sprintf(this->output_buffer_, "Iteration %d\n",
                      ++this->iterations_);
      this->bytes_to_send_ =
          static_cast<ssize_t>(ACE_OS::strlen(this->output_buffer_));
      this->bytes_sent_ = 0;
    }
    if (this->bytes_sent_ >= this->bytes_to_send_) {
      this->bytes_to_send_ = -1;
      this->bytes_sent_ = -1;
      this->reactor()->remove_handler(this, ACE_Event_Handler::WRITE_MASK |
                                                ACE_Event_Handler::DONT_CALL);
      return 0;
    }
    char *buf = this->output_buffer_ + this->bytes_sent_;
    int bytes_sent = this->peer().send(buf, 1);
    if (bytes_sent < 0) {
      this->bytes_sent_ = this->bytes_to_send_;
      ACE_ERROR_RETURN((LM_ERROR, "%p\n", "send"), 0);
    }
    this->bytes_sent_ += bytes_sent;
    return 0;
  }

  int handle_input(ACE_HANDLE) {
    char buf[64];
    int bytesReceived;
    if ((bytesReceived = this->peer_.recv(buf, sizeof(buf) - 1)) < 1) {
      ACE_DEBUG((LM_INFO,
                 "Client handle_input: "
                 "Received %d bytes. Leaving.\n",
                 bytesReceived));
      return -1;
    }
    buf[bytesReceived] = 0;
    ACE_DEBUG((LM_INFO, "Client handle_input: %d: %s\n", bytesReceived, buf));
    // Re-enable handle_output in three seconds.
    ACE_Time_Value one_shot(3);
    this->reactor()->schedule_timer(this, 0, one_shot);
    return 0;
  }

  int handle_timeout(const ACE_Time_Value &, const void *) {
    if (this->iterations_ >= MAX_ITERATIONS) {
      this->reactor()->end_event_loop();
    } else {
      this->reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);
    }
    return 0;
  }

private:
  int iterations_;
  int bytes_sent_;
  ssize_t bytes_to_send_;
  char output_buffer_[1024];
};

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  Client *client = new Client();
  ACE_INET_Addr addr("localhost", 9090);
  if (client->open(addr) == -1) {
    ACE_DEBUG((LM_DEBUG,
               "(%P|%t %d)"
               "error connecting to server\n",
               ACE_OS::last_error()));
    return -1;
  }

  ACE_Reactor::instance()->register_handler(
      client, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK);

  ACE_Reactor::instance()->run_reactor_event_loop();
  return 0;
}
