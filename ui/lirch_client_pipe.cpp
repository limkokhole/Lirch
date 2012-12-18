#include "ui/lirch_client_pipe.h"

// These are rather trivial
LirchClientPipe::LirchClientPipe() :
    client_state(State::BEFORE), core_thread(nullptr)
{
    QChar double_exclamation_mark(0x203C);
    QChar interrobang(0x203D);
    notify_prefix += double_exclamation_mark;
    notify_prefix += interrobang;
}

LirchClientPipe::~LirchClientPipe() { }

void LirchClientPipe::load(std::thread *t) {
    core_thread = t;
}

// A client pipe is ready for a duration
bool LirchClientPipe::ready() const {
    return client_state == State::DURING;
}

QString LirchClientPipe::name() const {
    return client_name;
}

#ifndef NDEBUG
#include <QDebug>
#endif

// The client pipe writes outbound messages
void LirchClientPipe::send(message m) {
    if (ready()) {
        #ifndef NDEBUG
        qDebug() << tr("Mediator for '%1' forwarded '%2' message").arg(client_name, QString::fromStdString(m.type));
        #endif
        client_pipe.write(m);
    }
}

// The client pipe signals on nick changes
void LirchClientPipe::nick(changed_nick_message m) {
    emit nick_changed(m.newNick, m.wasDefault);
}

// The client pipe signals on nick changes
void LirchClientPipe::channel(set_channel_message m) {
    emit set_channel(m.channel);
}

// The client pipe signals on nick changes
void LirchClientPipe::channel(leave_channel_message m) {
    emit leave_channel(m.channel);
}

// The client_pipe signals on userlist updates
void LirchClientPipe::userlist(userlist_message m) {
    QMap<QString, QSet<QString>> data;
    for (auto &entry : m.statuses) {
        QString nick = entry.second.nick;
        for (auto &channel : entry.second.channels) {
		data[channel].insert(nick);
        }
    }
    emit userlist_updated(data);
}

// The client pipe alerts the client of inbound messages
void LirchClientPipe::display(display_message m) {
    QString text;
    switch (m.subtype) {
        case display_message_subtype::NORMAL:
            text = tr("<%1> %2").arg(m.nick, m.contents);
            break;
        case display_message_subtype::ME:
            text = tr("* %1 %2").arg(m.nick, m.contents);
            break;
        case display_message_subtype::NOTIFY:
            text = notify_prefix + tr(" %1 %2").arg(m.nick, m.contents);
            break;
        default:
            text = tr("? %1 %2").arg(m.nick, m.contents);
    }
    emit display_received(m.channel, m.nick, text);
}

// The client pipe signals the UI when it the plugin is run
void LirchClientPipe::open(plugin_pipe pipe, QString name) {
    client_name = name;
    client_pipe = pipe;
    client_state = State::DURING;
    emit run(this);
}

// The client pipe signals the UI when the plugin is shutdown
void LirchClientPipe::close(QString reason) {
    client_state = State::AFTER;
    emit shutdown(tr("%1 was closed for %2").arg(client_name, reason));
}

// The client pipe wait on the core thread when the UI is closed
void LirchClientPipe::join() {
    send(message::create<core_quit_message>());
    // Wait for the core to exit
    if (core_thread) {
        core_thread->join();
    }
}

