#ifndef DECK_PREVIEW_WIDGET_H
#define DECK_PREVIEW_WIDGET_H

#include "../../../../../deck/deck_loader.h"
#include "../../cards/deck_preview_card_picture_widget.h"
#include "../visual_deck_storage_widget.h"
#include "deck_preview_color_identity_widget.h"
#include "deck_preview_deck_tags_display_widget.h"

#include <QVBoxLayout>
#include <QWidget>

class VisualDeckStorageWidget;
class DeckPreviewDeckTagsDisplayWidget;
class DeckPreviewWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit DeckPreviewWidget(QWidget *_parent,
                               VisualDeckStorageWidget *_visualDeckStorageWidget,
                               const QString &_filePath);
    QString getColorIdentity();

    VisualDeckStorageWidget *visualDeckStorageWidget;
    QVBoxLayout *layout;
    QString filePath;
    DeckLoader *deckLoader;
    DeckPreviewCardPictureWidget *bannerCardDisplayWidget;
    DeckPreviewColorIdentityWidget *colorIdentityWidget;
    DeckPreviewDeckTagsDisplayWidget *deckTagsDisplayWidget;
    bool filteredBySearch = false;
    bool filteredByColor = false;
    bool filteredByTags = false;
    bool checkVisibility() const;

signals:
    void deckPreviewClicked(QMouseEvent *event, DeckPreviewWidget *instance);
    void deckPreviewDoubleClicked(QMouseEvent *event, DeckPreviewWidget *instance);
    void visibilityUpdated();

public slots:
    void setFilePath(const QString &filePath);
    void refreshBannerCardText();
    void imageClickedEvent(QMouseEvent *event, DeckPreviewCardPictureWidget *instance);
    void imageDoubleClickedEvent(QMouseEvent *event, DeckPreviewCardPictureWidget *instance);
    void initializeUi(bool deckLoadSuccess);
    void updateVisibility();
};

#endif // DECK_PREVIEW_WIDGET_H
