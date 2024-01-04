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

	class FileSystemItem
	{
	public:
		FileSystemItem(const FileSystemItemType type) : type_(type), size_(0) {
			dbg("Files: Creating item of type %d", type_);
		}
		FileSystemItem(const FileSystemItemType type, const std::string& name) : name_(name), type_(type), size_(0) {}

		const std::string& GetName() const { return name_; }
		void SetName(const std::string name);
		std::string GetPath() const;
		const std::string& GetDate() const { return date_; }
		void SetDate(const std::string& date) { date_ = date; }
		size_t GetSize() const { return size_; }
		void SetSize(const size_t size) { size_ = size; }
		FileSystemItemType GetType() const { return type_; }

		~FileSystemItem();
	private:
		std::string name_;
		FileSystemItemType type_;
		size_t size_;
		std::string date_;
	};

	class File : public FileSystemItem
	{
	public:
		File() : FileSystemItem(FileSystemItemType::file) {}
		File(const std::string& name) : FileSystemItem(FileSystemItemType::file, name) {}
	};


	class Folder : public FileSystemItem
	{
	public:
		Folder() : FileSystemItem(FileSystemItemType::folder) {}
		Folder(const std::string& name) : FileSystemItem(FileSystemItemType::folder, name) {}
	};

	File* AddFileAt(const size_t index);
	Folder* AddFolderAt(const size_t index);
	const size_t GetItemCount();
	FileSystemItem* GetItem(const size_t index);
	File* GetFile(const std::string& name);
	Folder* GetSubFolder(const std::string& name);
	void SetCurrentDir(const std::string& path);
	std::string GetParentDirPath();
	std::string GetCurrentDirName();
	std::string& GetCurrentDirPath();
	void ListItems();
	void ClearFileSystem();
} /* namespace OM */



#endif /* JNI_LOGIC_OBJECTMODEL_FILES_HPP_ */
