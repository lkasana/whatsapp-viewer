#include <wx/listctrl.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "DatabasePanel.h"
#include "MessagePanelList.h"

#include "../whatsapp/Database.h"

namespace UI {

wxBEGIN_EVENT_TABLE(DatabasePanel, wxPanel)
  EVT_LIST_ITEM_SELECTED(XRCID("chats"), DatabasePanel::OnDisplayChat)
wxEND_EVENT_TABLE()

DatabasePanel::DatabasePanel(wxWindow *parent, std::unique_ptr<WhatsApp::Database> database)
    : database(std::move(database)), selectedChat(std::nullopt) {
  wxXmlResource::Get()->LoadPanel(this, parent, _("DatabasePanel"));
  wxXmlResource::Get()->AttachUnknownControl("messages", new MessagePanelList(this));

  chats = this->database->loadChats();
  updateChats();
}

void DatabasePanel::OnDisplayChat(wxListEvent &event) {
  WhatsApp::Chat &chat = *reinterpret_cast<WhatsApp::Chat *>(event.GetData());
  openChat(chat);
}

void DatabasePanel::updateChats() {
  wxListCtrl *chatControl = XRCCTRL(*this, "chats", wxListCtrl);
  chatControl->DeleteAllItems();

  for (auto &chat : chats) {
    wxListItem item;
    item.SetId(chatControl->GetItemCount());
    item.SetData(&chat);
    item.SetText(chat.getId());
    chatControl->InsertItem(item);
  }

  chatControl->SetColumnWidth(0, wxLIST_AUTOSIZE);
}

void DatabasePanel::openChat(WhatsApp::Chat &chat) {
  chat.setMessages(database->loadMessages(chat));
  selectedChat = std::make_optional(&chat);

  auto *messages = XRCCTRL(*this, "messages", MessagePanelList);
  messages->clear();
  messages->setList(chat.getMessages());
}

} // namespace UI
