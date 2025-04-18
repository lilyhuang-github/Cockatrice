#ifndef EDHREC_COMMANDER_API_RESPONSE_COMMANDER_DETAILS_DISPLAY_WIDGET_H
#define EDHREC_COMMANDER_API_RESPONSE_COMMANDER_DETAILS_DISPLAY_WIDGET_H

#include "../../../../../ui/widgets/cards/card_info_picture_widget.h"
#include "../../api_response/commander/edhrec_commander_api_response_commander_details.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class EdhrecCommanderResponseCommanderDetailsDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EdhrecCommanderResponseCommanderDetailsDisplayWidget(
        QWidget *parent,
        const EdhrecCommanderApiResponseCommanderDetails &_commanderDetails);
    void retranslateUi();

public slots:
    void actRequestComboNavigation();
    void actRequestAverageDeckNavigation();
signals:
    void requestUrl(QString url);

private:
    QLabel *label;
    QLabel *salt;
    QPushButton *comboPushButton;
    QPushButton *averageDeckPushButton;
    QVBoxLayout *layout;
    CardInfoPictureWidget *commanderPicture;
    EdhrecCommanderApiResponseCommanderDetails commanderDetails;
};

#endif // EDHREC_COMMANDER_API_RESPONSE_COMMANDER_DETAILS_DISPLAY_WIDGET_H
