/*
 * Files.hpp
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_OBJECTMODEL_FILES_HPP_
#define JNI_LOGIC_OBJECTMODEL_FILES_HPP_

#include <string>
#include <vector>

namespace OM
{
	enum class FileSystemItemType
	{
		file,
		folder,
	};

	class Folder;

	class FileSystemItem
	{
	public:
		FileSystemItem(const FileSystemItemType type) : pParent_(nullptr), type_(type) {}
		FileSystemItem(const FileSystemItemType type, Folder* parent) : pParent_(parent), type_(type) {}
		FileSystemItem(const FileSystemItemType type, const std::string& name) : name_(name), pParent_(nullptr), type_(type) {}
		FileSystemItem(const FileSystemItemType type, const std::string& name, Folder* parent) : name_(name), pParent_(parent), type_(type) {}

		const std::string& GetName() const { return name_; }
		void SetName(const std::string& name) { name_ = name; }
		std::string GetPath() const;
		FileSystemItemType GetType() const { return type_; }

		virtual ~FileSystemItem();
	private:
		std::string name_;
		Folder* pParent_;
		FileSystemItemType type_;
	};

	class File : public FileSystemItem
	{
	public:
		File(Folder* parent) : FileSystemItem(FileSystemItemType::file, parent), size_(0) {}
		File(const std::string& name, Folder* parent) : FileSystemItem(FileSystemItemType::file, name, parent), size_(0) {}
		File(const std::string& name, const size_t size, Folder* parent) : FileSystemItem(FileSystemItemType::file, name, parent), size_(size) {}

		const std::string& GetDate() const { return date_; }
		void SetDate(const std::string& date) { date_ = date; }
		size_t GetSize() const { return size_; }
		void SetSize(const size_t size) { size_ = size; }

	private:
		size_t size_;
		std::string date_;
	};


	class Folder : public FileSystemItem
	{
	public:
		Folder(Folder* parent) : FileSystemItem(FileSystemItemType::folder, parent) {}
		Folder(const std::string& name) : FileSystemItem(FileSystemItemType::folder, name) {}
		Folder(const std::string& name, Folder* parent) : FileSystemItem(FileSystemItemType::folder, name, parent) {}
		File* AddFileAt(const size_t index)
		{
			if (index < items_.size())
			{
				dbg("Deleting item[%d]", index);
				delete items_[index];
				items_[index] = nullptr;
			}
			File* file = new File(this);
			items_.insert(items_.begin() + index - 1, file);
			return file;
		}

		File* AddFile(const std::string& name)
		{
			dbg("Files: Adding file %s to %s", name.c_str(), GetName().c_str());
			items_.push_back(new File(name, this));
			return (File*)items_.back();
		}

		Folder* AddFolderAt(const size_t index)
		{
			if (index < items_.size())
			{
				dbg("Deleting item[%d]", index);
				delete items_[index];
				items_[index] = nullptr;
			}
			Folder* folder = new Folder(this);
			items_.insert(items_.begin() + index - 1, folder);
			return folder;
		}

		Folder* AddFolder(const std::string& name)
		{
			dbg("Files: Adding folder %s to %s", name.c_str(), GetName().c_str());
			items_.push_back(new Folder(name, this));
			return (Folder*)items_.back();
		}

		File* GetFile(const std::string& name);
		Folder* GetSubFolder(const std::string& name);
		File* GetOrCreateFile(const std::string& name);
		Folder* GetOrCreateSubFolder(const std::string& name);
		const size_t GetItemCount() const { return items_.size(); }
		FileSystemItem* GetItem(const size_t index)
		{
			if (index >= GetItemCount())
				return nullptr;
			return items_[index];
		}

		void Clear()
		{
			dbg("Files: clearing folder %s", GetPath().c_str());
			ListItems();
			for (auto item : items_)
			{
				if (item == nullptr)
					continue;
				dbg("Files: deleting item %s", item->GetName().c_str());
				delete item;
			}
			items_.clear();
		}

		void ListItems()
		{
			std::string itemStr;
			for (auto item : items_)
			{
				itemStr += item->GetName();
				itemStr += " ";
			}
			dbg("Files: item list = %s", itemStr.c_str());
		}
		~Folder() { Clear(); }

	private:
		std::vector<FileSystemItem*> items_;
	};

	Folder* GetRootFolder();
	void SetCurrentDir(const std::string& path);
	std::string& GetCurrentDirPath();
	Folder* GetCurrentFolder();
} /* namespace OM */



#endif /* JNI_LOGIC_OBJECTMODEL_FILES_HPP_ */
