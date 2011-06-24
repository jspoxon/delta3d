/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Jeff P. Houde
 */

#ifndef DIRECTORQT_UNDO_IMMEDIATE_LINK_EVENT
#define DIRECTORQT_UNDO_IMMEDIATE_LINK_EVENT

#include <dtDirectorQt/undoevent.h>

namespace dtDirector
{
   class InputLink;
   class OutputLink;
   class ValueLink;

   /**
    * Undo event for changing the visibility on a node link.
    */
   class UndoImmediateLinkEvent: public UndoEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor     The editor.
       * @param[in]  nodeID     The ID of the node or macro.
       * @param[in]  linkType   The type of link (0=Input, 1=Output, 2=Value).
       * @param[in]  linkName   The name of the link.
       * @param[in]  enabled    Whether the link is set to immediate mode or not.
       */
      UndoImmediateLinkEvent(DirectorEditor* editor, const dtCore::UniqueId& nodeID, int linkType, const std::string& linkName, bool enabled);

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

   protected:

      /**
       * Destructor.
       */
      virtual ~UndoImmediateLinkEvent();

      InputLink* GetInputLink();
      OutputLink* GetOutputLink();
      ValueLink* GetValueLink();

      dtCore::UniqueId  mID;
      int               mLinkType;
      std::string       mLinkName;
      bool              mEnabled;
   };
}

#endif // DIRECTORQT_UNDO_IMMEDIATE_LINK_EVENT