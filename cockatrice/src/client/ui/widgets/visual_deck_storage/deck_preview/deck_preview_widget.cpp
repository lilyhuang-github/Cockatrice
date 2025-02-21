#include "deck_preview_widget.h"

#include "../../../../../game/cards/card_database_manager.h"
#include "../../../../../settings/cache_settings.h"
#include "../../cards/deck_preview_card_picture_widget.h"
#include "deck_preview_deck_tags_display_widget.h"

#include <QFileInfo>
#include <QMouseEvent>
#include <QSet>
#include <QVBoxLayout>

DeckPreviewWidget::DeckPreviewWidget(QWidget *_parent,
                                     VisualDeckStorageWidget *_visualDeckStorageWidget,
                                     const QString &_filePath)
    : QWidget(_parent), visualDeckStorageWidget(_visualDeckStorageWidget), filePath(_filePath),
      colorIdentityWidget(nullptr), deckTagsDisplayWidget(nullptr)
{
    layout = new QVBoxLayout(this);
    setLayout(layout);

    deckLoader = new DeckLoader();
    connect(deckLoader, &DeckLoader::loadFinished, this, &DeckPreviewWidget::initializeUi);
    connect(deckLoader, &DeckLoader::loadFinished, visualDeckStorageWidget->tagFilterWidget,
            &VisualDeckStorageTagFilterWidget::refreshTags);
    deckLoader->loadFromFileAsync(filePath, DeckLoader::getFormatFromName(filePath), false);

    bannerCardDisplayWidget = new DeckPreviewCardPictureWidget(this);

    connect(bannerCardDisplayWidget, &DeckPreviewCardPictureWidget::imageClicked, this,
            &DeckPreviewWidget::imageClickedEvent);
    connect(bannerCardDisplayWidget, &DeckPreviewCardPictureWidget::imageDoubleClicked, this,
            &DeckPreviewWidget::imageDoubleClickedEvent);

    connect(&SettingsCache::instance(), &SettingsCache::visualDeckStorageShowTagsOnDeckPreviewsChanged, this,
            &DeckPreviewWidget::updateTagsVisibility);

    layout->addWidget(bannerCardDisplayWidget);
}

void DeckPreviewWidget::initializeUi(const bool deckLoadSuccess)
{
    if (!deckLoadSuccess) {
        return;
    }
    auto bannerCard = deckLoader->getBannerCard().first.isEmpty()
                          ? CardInfoPtr()
                          : CardDatabaseManager::getInstance()->getCardByNameAndProviderId(
                                deckLoader->getBannerCard().first, deckLoader->getBannerCard().second);

    bannerCardDisplayWidget->setCard(bannerCard);
    bannerCardDisplayWidget->setOverlayText(
        deckLoader->getName().isEmpty() ? QFileInfo(deckLoader->getLastFileName()).fileName() : deckLoader->getName());
    bannerCardDisplayWidget->setFontSize(24);
    setFilePath(deckLoader->getLastFileName());

    colorIdentityWidget = new DeckPreviewColorIdentityWidget(this, getColorIdentity());
    deckTagsDisplayWidget = new DeckPreviewDeckTagsDisplayWidget(this, deckLoader);
    updateTagsVisibility(SettingsCache::instance().getVisualDeckStorageShowTagsOnDeckPreviews());

    layout->addWidget(colorIdentityWidget);
    layout->addWidget(deckTagsDisplayWidget);
}

void DeckPreviewWidget::updateVisibility()
{
    if (isVisible() != checkVisibility()) {
        setHidden(!checkVisibility());
        emit visibilityUpdated();
    }
}

bool DeckPreviewWidget::checkVisibility() const
{
    if (filteredBySearch || filteredByColor || filteredByTags) {
        return false;
    }
    return true;
}

void DeckPreviewWidget::updateTagsVisibility(bool visible)
{
    if (!deckTagsDisplayWidget) {
        return;
    }

    if (visible) {
        deckTagsDisplayWidget->setVisible(true);
    } else {
        deckTagsDisplayWidget->setHidden(true);
    }
}

QString DeckPreviewWidget::getColorIdentity()
{
    QStringList cardList = deckLoader->getCardList();
    if (cardList.isEmpty()) {
        return {};
    }

    QSet<QChar> colorSet; // A set to collect unique color symbols (e.g., W, U, B, R, G)

    for (const QString &cardName : cardList) {
        CardInfoPtr currentCard = CardDatabaseManager::getInstance()->getCard(cardName);
        if (currentCard) {
            QString colors = currentCard->getColors(); // Assuming this returns something like "WUB"
            for (const QChar &color : colors) {
                colorSet.insert(color);
            }
        }
    }

    // Ensure the color identity is in WUBRG order
    QString colorIdentity;
    const QString wubrgOrder = "WUBRG";
    for (const QChar &color : wubrgOrder) {
        if (colorSet.contains(color)) {
            colorIdentity.append(color);
        }
    }

    return colorIdentity;
}

void DeckPreviewWidget::setFilePath(const QString &_filePath)
{
    filePath = _filePath;
}

void DeckPreviewWidget::refreshBannerCardText()
{
    bannerCardDisplayWidget->setOverlayText(
        deckLoader->getName().isEmpty() ? QFileInfo(deckLoader->getLastFileName()).fileName() : deckLoader->getName());
}

void DeckPreviewWidget::imageClickedEvent(QMouseEvent *event, DeckPreviewCardPictureWidget *instance)
{
    Q_UNUSED(instance);
    emit deckPreviewClicked(event, this);
}

void DeckPreviewWidget::imageDoubleClickedEvent(QMouseEvent *event, DeckPreviewCardPictureWidget *instance)
{
    Q_UNUSED(instance);
    emit deckPreviewDoubleClicked(event, this);
}