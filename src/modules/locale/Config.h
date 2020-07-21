/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2020 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *   License-Filename: LICENSE
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCALE_CONFIG_H
#define LOCALE_CONFIG_H

#include "LocaleConfiguration.h"

#include "Job.h"
#include "locale/TimeZone.h"

#include <QObject>

#include <memory>

class Config : public QObject
{
    Q_OBJECT
    Q_PROPERTY( const QStringList& supportedLocales READ supportedLocales CONSTANT FINAL )
    Q_PROPERTY( CalamaresUtils::Locale::CStringListModel* zonesModel READ zonesModel CONSTANT FINAL )
    Q_PROPERTY( CalamaresUtils::Locale::CStringListModel* regionModel READ regionModel CONSTANT FINAL )

    Q_PROPERTY( const CalamaresUtils::Locale::TZZone* currentLocation READ currentLocation WRITE setCurrentLocation
                    NOTIFY currentLocationChanged )

    Q_PROPERTY( QString currentLocationStatus READ currentLocationStatus NOTIFY currentLanguageStatusChanged )
    Q_PROPERTY( QString currentLanguageStatus READ currentLanguageStatus NOTIFY currentLanguageStatusChanged )
    Q_PROPERTY( QString currentLCStatus READ currentLCStatus NOTIFY currentLCStatusChanged )

public:
    Config( QObject* parent = nullptr );
    ~Config();

    void setConfigurationMap( const QVariantMap& );
    Calamares::JobList createJobs();

    // Underlying data for the models
    const CalamaresUtils::Locale::CStringPairList& timezoneData() const;

    /** @brief The currently selected location (timezone)
     *
     * The location is a pointer into the date that timezoneData() returns.
     * It is possible to return nullptr, if no location has been picked yet.
     */
    const CalamaresUtils::Locale::TZZone* currentLocation() const { return m_currentLocation; }

    /// locale configuration (LC_* and LANG) based solely on the current location.
    LocaleConfiguration automaticLocaleConfiguration() const;
    /// locale configuration that takes explicit settings into account
    LocaleConfiguration localeConfiguration() const;

    /// The human-readable description of what timezone is used
    QString currentLocationStatus() const;
    /// The human-readable description of what language is used
    QString currentLanguageStatus() const;
    /// The human-readable description of what locale (LC_*) is used
    QString currentLCStatus() const;

    const QStringList& supportedLocales() const { return m_localeGenLines; }
    CalamaresUtils::Locale::CStringListModel* regionModel() const { return m_regionModel.get(); }
    CalamaresUtils::Locale::CStringListModel* zonesModel() const { return m_zonesModel.get(); }

public Q_SLOTS:
    /// Set a language by user-choice, overriding future location changes
    void setLanguageExplicitly( const QString& language );
    /// Set LC (formats) by user-choice, overriding future location changes
    void setLCLocaleExplicitly( const QString& locale );

    /** @brief Sets a location by name
     *
     * @p region should be "America" or the like, while @p zone
     * names a zone within that region.
     */
    void setCurrentLocation( const QString& region, const QString& zone );
    /** @brief Sets a location by pointer
     *
     * Pointer should be within the same model as the widget uses.
     * This can update the locale configuration -- the automatic one
     * follows the current location, and otherwise only explicitly-set
     * values will ignore changes to the location.
     */
    void setCurrentLocation( const CalamaresUtils::Locale::TZZone* location );

signals:
    void currentLocationChanged( const CalamaresUtils::Locale::TZZone* location ) const;
    void currentLocationStatusChanged( const QString& ) const;
    void currentLanguageStatusChanged( const QString& ) const;
    void currentLCStatusChanged( const QString& ) const;

private:
    /// A list of supported locale identifiers (e.g. "en_US.UTF-8")
    QStringList m_localeGenLines;

    /// The regions (America, Asia, Europe ..)
    std::unique_ptr< CalamaresUtils::Locale::CStringListModel > m_regionModel;
    /// The zones for the current region (e.g. America/New_York)
    std::unique_ptr< CalamaresUtils::Locale::CStringListModel > m_zonesModel;

    /// The location, points into the timezone data
    const CalamaresUtils::Locale::TZZone* m_currentLocation = nullptr;

    /** @brief Specific locale configurations
     *
     * "Automatic" locale configuration based on the location (e.g.
     * Europe/Amsterdam means Dutch language and Dutch locale) leave
     * this empty; if the user explicitly sets something, then
     * this configuration is non-empty and takes precedence over the
     * automatic location settings (so a user in Amsterdam can still
     * pick Ukranian settings, for instance).
     */
    LocaleConfiguration m_selectedLocaleConfiguration;

    /** @brief Should we adjust the "live" timezone when the location changes?
     *
     * In the Widgets UI, clicking around on the world map adjusts the
     * timezone, and the live system can be made to follow that.
     */
    bool m_adjustLiveTimezone;
};


#endif
