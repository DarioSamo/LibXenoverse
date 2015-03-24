#include "EMDOgre.h"
#include "ESKOgre.h"
#include "EANOgre.h"
#include "FileTreeItemWidget.h"

FileTreeItemWidget::FileTreeItemWidget(QTreeWidget *parent) : QTreeWidgetItem(parent) {
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
}

SkeletonItemWidget::SkeletonItemWidget(ESKOgre *data, QTreeWidget *parent) : FileTreeItemWidget(parent) {
	data_ptr = data;
	setText(0, (data->getName() + " (Skeleton)").c_str());
	setIcon(0, QIcon(":/icons/esk.png"));
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled);

	type = ItemSkeleton;
}

SkeletonItemWidget::~SkeletonItemWidget() {
	data_ptr->tagForDeletion();
}

ModelPackItemWidget::ModelPackItemWidget(EMDOgre *data, QTreeWidget *parent) : FileTreeItemWidget(parent) {
	data_ptr = data;
	updateText();
	setIcon(0, QIcon(":/icons/emd.png"));
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsDragEnabled);
	setExpanded(true);

	type = ItemModelPack;
}

ModelPackItemWidget::~ModelPackItemWidget() {
	data_ptr->tagForDeletion();
}

void ModelPackItemWidget::updateText() {
	if (data_ptr) {
		if (data_ptr->getSkeleton()) {
			setText(0, (data_ptr->getName() + " (Model Pack / Skinned)").c_str());
		}
		else {
			setText(0, (data_ptr->getName() + " (Model Pack / No Skin)").c_str());
		}
	}
}

AnimationPackItemWidget::AnimationPackItemWidget(EANOgre *data, QTreeWidget *parent) : FileTreeItemWidget(parent) {
	data_ptr = data;
	setText(0, (data->getName() + " (Animation Pack)").c_str());
	setIcon(0, QIcon(":/icons/animation_pack.png"));
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	type = ItemAnimationPack;

	vector<EANAnimation> &animations = data->getAnimations();
	for (size_t i = 0; i < animations.size(); i++) {
		AnimationItemWidget *animation_item = new AnimationItemWidget(&animations[i], NULL);
		addChild(animation_item);
	}

	setExpanded(true);
}

AnimationItemWidget::AnimationItemWidget(EANAnimation *data, QTreeWidget *parent) : FileTreeItemWidget(parent) {
	data_ptr = data;
	setText(0, (data->getName()).c_str());
	setIcon(0, QIcon(":/icons/animation.png"));
	setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	type = ItemAnimation;
}